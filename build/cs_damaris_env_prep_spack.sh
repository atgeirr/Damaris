#!/bin/bash
# Author: Josh Bowden, INRIA
# Description:
# This script configures Damaris via camke, using pre-installed libraries that were installed via Spack.
# It is specific to installing Damaris with in-sityuu vizulisation support with Catalyst using a Spack 
# installed Paraview library.
#
# Before running this script, please make sure that C/C++/Fortran compilers are installed on the target machine.
# i.e. prerequisites for Debian:
# >sudo apt-get install gfortran gcc g++ git-core
#
# Spack can be installed:
# >git clone https://github.com/spack/spack.git
#
# Before installing Paraview, we may want to specify pre-installed LLVM and Mesa libraries:
# >cat ~/.spack/packages.yaml
#   ---
#   packages:
#   all:
#     variants: ~shared  # make ALL libs install as static libs
#   packages:
#     python:
#       version: [3, 2]
#    
## version: clang --version
#    llvm: 
#      externals:
#      - spec: llvm@7.0.0
#        prefix: /usr/lib/llvm-7
#      version: [7.0]
#      buildable: False
#    
## may need: sudo apt-get install libssl-dev
## Forr _ssl module error: add full path to /usr/bin/python3 to the spack script
## to specify the system python 3 to be used due to _ssl import error 
## due to spack installed python
## version: openssl version
#    openssl:
#      externals:
#      - spec: openssl@1.1.1
#        prefix: /usr/lib/x86_64-linux-gnu
#      buildable: False
#
## version: find / -type f -name ncurses 2> /dev/null 
#    ncurses:
#      externals:
#      - spec:   ncurses@6.1.2
#       prefix: /usr/lib/x86_64-linux-gnu
#      version: [6.1]
#      buildable: False

# 
# Also set the install path
# >cat ~/.spack/config.yaml set:
# install_pat_scheme:  ${ARCHITECTURE}/${COMPILERNAME}-${COMPILERVER}/${PACKAGE}-${VERSION}-${HASH:5} 
#

# Dependencies can be installed using Spack:
# spack install xsd ^xerces-c +cxxstd=11
# spack install boost +thread+log+filesystem+date_time @1.67:
# spack install cppunit cxxstd=11
# N.B. Prior to Paraview 5.7, there was a seperate Catalyst spack option for minimalist installation
# spack install paraview@5.8.0~cuda~examples+hdf5+kits+mpi+opengl2+osmesa+plugins~python+python3~qt+shared build_type=Release cuda_arch=none
#
# N.B. Do not install hdf5 or openmpi as Paraview installs them as dependencies and this script will find and use the dependents



############################################################
## User supplied variables
############################################################

 echo -e "--- Setting up user defined variables  ---------------------------------------------------------------\n"
# Where this script has been placed
SCRIPTDIR=/home/jbowden/C_S/damaris_build
# Where the source code for Damaris is
REPODIR=/home/jbowden/C_S/damaris-development
# The build directory that will be created in the SCRIPTDIR direcory
BUILDDIR=damaris-build-sharedlib-examples-dbg
# The installation direcory
INSTALL_PREFIX=/opt/damaris/shared_dbg


# CMake options (other options can be changed at the cmake... command below)
# Options: ON | OFF
SHLIBS_ON_OFF=ON
# Build examples
# Currently, both the build and the install of the examples are broken
# due to examples/plugin/ not finding vtkDataObject.h in build and xsd/cxx/config.hxx in install
EXAMPLES_ON_OFF=ON
# Options: Debug | Release | MinSizeRel | RelWithDebInfo
CMAKE_BUILD_TYPE=Debug
# Regenerate the xml model in C using XSD
REGEN_ON_OFF=OFF


################
## Use spack hashes to specify exact library to pass to cmake
################
# Once installed by Spack library hashes can be looked at using:
# >spack find -vl paraview
# Then use the hash of the particular install: 
# hxuqw2z paraview@5.8.0~cuda~examples~hdf5+kits+mpi+opengl2+osmesa+plugins~python+python3~qt+shared build_type=Release cuda_arch=none
# ^   
# >spack dependencies --installed /hxuqw2z
# >spack dependents --installed /hxuqw2z
################

XSD_HASH=areex6c
BOOST_HASH=4qfvorx
CPPUNIT_HASH=jywsrhy
# Paraview has dependent libs OpenMPI HDF5 and Python
# They should *not be overridden* by other versions using: spack load <other version>
##  N.B. downstream med spec seems to want hdf5@1.10.2
##  Damaris installed hdf5@10.1.6 with the Paraview installation
##  So perhaps get paraview to use to use ^hdf5@1.10.2 
##  *or* edit spack definition for med -> depends_on('hdf5@1.10.2:+mpi', when='@4.0.0')
# spack install med ^openmpi/$OPENMPI_HASH  ^hdf5/$HDF_HASH_PV
# Med could be replaced as a file format though.
PARAVIEW_HASH=e6s2at7

############################################################
## End of user supplied variables
############################################################



############################################################
## Shell Function definitions
############################################################
# Function to not exit shell if script is sourced:
# Usage: ret_or_exit -1 || return $?
# Will exit if script is executed and return if script is sourced
function ret_or_exit {
 if [[ x"${BASH_SOURCE[0]}" == x"$0" ]] ; then
   exit $1
 else
   return $1
 fi
}

function exit_fail {
    echo "ERROR: $1 could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl $2)"
    ret_or_exit -1 || return $?
}


function exit_no_dir {
  if [[ ! -d "$1" ]] ; then
    echo "ERROR: $2 could not be found. Please check that xsd has been installed via Spack. And check the hash used (spack find -vl $3)"
    ret_or_exit -1 || return $?
  fi
}
############################################################
## End of Shell Function definitions
############################################################



# Test if the build directory exists
if [[  -d "$SCRIPTDIR/$BUILDDIR" ]] ; then
  echo "ERROR: Exiting - Build directory exists: $SCRIPTDIR/$BUILDDIR "
  sleep 3
  ret_or_exit -1 || return $?
fi
mkdir -p $SCRIPTDIR/$BUILDDIR
cd $SCRIPTDIR/$BUILDDIR
 

# Test if the reopository source directory exists
if [[ ! -d "$REPODIR" ]] ; then
  echo "ERROR: Source directory does not exist: $REPODIR "
  ret_or_exit -1 || return $?
fi


 echo -e "--- Setting up Spack  ---------------------------------------------------------------\n"
# Load the Spack environment
WHICHSPACK=`which spack`
if [[ -z "$WHICHSPACK" ]] ; then
. $SPACK_ROOT/share/spack/setup-env.sh
fi


# Get the hashes of the specific dependencies of the Paraview and XSD installs :
# spack dependencies does not use this yet: --format "{hash:7}"
OMPI_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep openmpi | sed "s| openmpi.*$||g")
NETCDF_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep netcdf | sed "s| netcdf.*$||g")  
HDF5_HASH=$(spack dependencies --installed /$NETCDF_HASH | grep hdf5 | sed "s| hdf5.*$||g")  
PYTHON_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep python | sed "s| python.*$||g")
XERCES_HASH=$(spack dependencies --installed /$XSD_HASH | grep xerces | sed "s| xerces.*$||g")

# Test if installation direcotry exists, if not make it (may need to be sudo?)
# if [[ ! -d "$INSTALL_PREFIX/bin" ]]; then
#   mkdir -p $INSTALL_PREFIX/bin
#   if [ $? -ne 0 ]; then
#     echo "ERROR: Could not make the installation directory: $INSTALL_PREFIX/bin"
#     echo "do you need sudo access?"
#     exit -1
#   fi
# fi

export PATH=$INSTALL_PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$INSTALL_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH



##############################################################
## Load Spack installed libraries
## Each load is tested to see if the library loaded successfully
##############################################################
spack unload -a

echo -e "--- Loading spack libraries  ------------------------------------------------------\n"

### N.B. Need to use cmake >=3.12, but not 3.18 as it has an issue finding openmpi
# cmake@3.14.1
spack load -r cmake@3.18.2  #.3%gcc@8.3.0 arch=linux-debian10-sandybridge
spack load -r boost/$BOOST_HASH   || exit_fail  boost/$BOOST_HASH  boost
spack load -r xsd/$XSD_HASH  || exit_fail  xsd/$XSD_HASH  xsd
spack load -r cppunit/$CPPUNIT_HASH  || exit_fail cppunit/$CPPUNIT_HASH -1
# This will load openmpi and hdf5 and python3 as dependencies
spack load -r paraview/$PARAVIEW_HASH || exit_fail  paraview/$PARAVIEW_HASH paraview


# spack load -r hdf5/$HDF5_HASH 
# if [ $? -ne 0 ] ; then
#  echo "ERROR: hdf5/$HDF5_HASHvariable is empty. Please check that it has been installed via Spack. And check the hash used (spack find -v hdf5)"
#  exit -1
#fi
#spack load -r openmpi/$OMPI_HASH 
#if [$? -ne 0 ] ; then
#  echo "ERROR: openmpi/$OMPI_HASH  could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -v openmpi)"
#  exit -1
#fi

echo -e "--- Getting Spack installation directories ----------------------------------------\n"

export BOOSTDIR=$(spack find -p boost/$BOOST_HASH | grep boost | sed "s|^.* ||g")
exit_no_dir "$BOOSTDIR" BOOSTDIR boost
export XSDDIR=$(spack find -p xsd/$XSD_HASH  | grep xsd | sed "s|^.* ||g")
exit_no_dir "$XSDDIR" XSDDIR xsd
export XERCESSDIR=$(spack find -p xerces-c/$XERCES_HASH  | grep xerces-c | sed "s|^.* ||g")
exit_no_dir "$XERCESSDIR" XERCESSDIR xerces-c
export CPPUNITDIR=$(spack find -p cppunit/$CPPUNIT_HASH  | grep cppunit | sed "s|^.* ||g")
exit_no_dir "$CPPUNITDIR" CPPUNITDIR cppunit
export HDF5DIR=$(spack find -p hdf5/$HDF5_HASH  | grep hdf5 | sed "s|^.* ||g")
exit_no_dir "$HDF5DIR" HDF5DIR hdf5
export SPACKMPIPATH=$(spack find -p openmpi/$OMPI_HASH | grep openmpi | sed "s|^.* ||g")
exit_no_dir "$SPACKMPIPATH" SPACKMPIPATH openmpi
export CATALYSTDIR=$(spack find -p paraview/$PARAVIEW_HASH | grep paraview | sed "s|^.* ||g")
exit_no_dir "$CATALYSTDIR" CATALYSTDIR paraview



# N.B. As this path is very long it seems the #! format of Linux will not work, so make a soft link to the actual python below
# export PYTHONEXE=$(spack find -p python/$PYTHON_HASH  | grep cppunit | sed "s|^.* ||g")/bin/python
# if [[ ! -f $INSTALL_PREFIX/bin/python3 ]] ; then
#   ln -s $PYTHONEXE $INSTALL_PREFIX/bin/python3
#   chmod 744 $INSTALL_PREFIX/bin/python3
# fi


# compiling and installing Damaris
 echo -e "--- COMPILING DAMARIS -------------------------------------------------------------\n"

 

 cmake --verbose=1  -Wno-dev $REPODIR -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
   -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
	-DBOOST_ROOT=$BOOSTDIR\
	-DXSD_ROOT=$XSDDIR \
	-DXercesC_ROOT=$XERCESSDIR \
	-DCMAKE_CXX_COMPILER=$SPACKMPIPATH/bin/mpicxx \
	-DCMAKE_C_COMPILER=$SPACKMPIPATH/bin/mpicc \
	-DCMAKE_Fortran_COMPILER=$SPACKMPIPATH/bin/mpifort  \
   -DENABLE_FORTRAN:BOOL=ON \
	-DENABLE_TESTS=OFF -DCppUnit_ROOT=$CPPUNITDIR \
   -DENABLE_EXAMPLES=$EXAMPLES_ON_OFF \
	-DBUILD_SHARED_LIBS=$SHLIBS_ON_OFF \
	-DENABLE_HDF5=ON -DHDF5_ROOT=$HDF5DIR \
   -DENABLE_CATALYST=ON -DParaView_DIR=$CATALYSTDIR \
   -DGENERATE_MODEL=$REGEN_ON_OFF \
   -DENABLE_VISIT=OFF
 

cd ..

echo -e ""

echo -e "--- FINISHED CONFIGURATION -----------------------------------------------------------\n"
echo -e "Now: "
echo -e "cd $BUILDDIR"
echo -e "make -j2"
echo -e "sudo make install"
 
