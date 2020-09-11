#!/bin/bash
# Author: Josh Bowden, INRIA
# Description:
# This script configures Damaris via camke, using pre-installed libraries that were installed via Spack.  
# Before running this script, please make sure that C/C++/Fortran
# compilers are installed on the target machine.

# prerequisites for debian
# sudo apt-get install gfortran gcc g++ git-core python-dev

# v1CMAKE=1
# mpich or openmpi
# v8VISIT=0 # visit is difficult to install using Spack (see notes in notebook)

# spack install xerces-c cxxstd=11
# spack install xsd
# spack install boost +thread+log+filesystem+date_time @1.67:
# spack install cppunit cxxstd=11
# spack install paraview@5.8.0~cuda~examples~hdf5+kits+mpi+opengl2+osmesa+plugins~python+python3~qt+shared build_type=Release cuda_arch=none
# # Do not need to install hdf5 or openmpi as Paraview installs them as a dependency
# # Not needed: spack install hdf5
# # Not needed: spack install openmpi

# Where this script has been placed
SCRIPTDIR=/home/jbowden/C_S/damaris_build/
# Where the source code for Damaris is
REPODIR=/home/jbowden/C_S/damaris-development
# The build directory that will be created in the SCRIPTDIR direcory
BUILDDIR=damaris-build-shlib-msr
# The installation direcory
INSTALL_PREFIX=/opt/damaris/local


# CMake options (other options can be changed at the cmake... command below)
# Options: ON | OFF
SHLIBS_ON_OFF=ON
# Options: Debug | Release | MinSizeRel | RelWithDebInfo
CMAKE_BUILD_TYPE=MinSizeRel




# Test if the build directory exists
if [[  -d "$SCRIPTDIR/$BUILDDIR" ]] ; then
  echo "ERROR: Exiting - Build directory exists: $SCRIPTDIR/$BUILDDIR "
  exit -1
fi
mkdir -p $SCRIPTDIR/$BUILDDIR
cd $SCRIPTDIR/$BUILDDIR
 

# Test if the reopository source directory exists
if [[ ! -d "$REPODIR" ]] ; then
  echo "ERROR: Source directory does not exist: $REPODIR "
  exit -1
fi




#prerequisites
#sudo su; apt-get install aptitude
#aptitude install freeglut3-dev libgl1-mesa-dev libdrm-dev

 echo -e "--- Setting up Spack  ---------------------------------------------------------------\n"
# Load the Spack environment
WHICHSPACK=`which spack`
if [[ -z "$WHICHSPACK" ]] ; then
. $SPACK_ROOT/share/spack/setup-env.sh
fi

# Once installed by Spack libraries hashes can be looked at
# Using:
# >spack find -vl paraview
# Then use the hash of the particular install: 
# hxuqw2z paraview@5.8.0~cuda~examples~hdf5+kits+mpi+opengl2+osmesa+plugins~python+python3~qt+shared build_type=Release cuda_arch=none
# >spack dependencies --installed /hxuqw2z
# >spack dependents --installed /hxuqw2z

# To find hashes of all installed versions of a library use:
# >spack find -vl paraview
XERCES_HASH=dafgo
XSD_HASH=77vz5
BOOST_HASH=qv72sst
CPPUNIT_HASH=jywsrhy

# Paraview has dependent libs OpenMPI HDF5 and Python
# They should *not be overridden* by other versions using: spack load <other version>
PARAVIEW_HASH=hxuqw2z
# Get the specific dependency library hashes:
OMPI_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep openmpi | sed "s| openmpi.*$||g")
NETCDF_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep netcdf | sed "s| netcdf.*$||g")  
HDF5_HASH=$(spack dependencies --installed /$NETCDF_HASH | grep hdf5 | sed "s| hdf5.*$||g")  
PYTHON_HASH=$(spack dependencies --installed /$PARAVIEW_HASH | grep python | sed "s| python.*$||g")



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
export PKG_CONFIG_PATH=$INSTALL_PREFIX/lib/damaris:$PKG_CONFIG_PATH



#########################################################
## Load Spack installed libraries
## Each load is tested to see if the library loaded successfully
#########################################################
spack unload -a

echo -e "--- Loading spack libraries  ------------------------------------------------------\n"

### N.B. Need to use cmake >=3.12, but not 3.18 as it has an issue finding openmpi
spack load -r cmake@3.14.1  #.3%gcc@8.3.0 arch=linux-debian10-sandybridge
spack load -r xerces-c/$XERCES_HASH
if [ $? -ne 0 ]; then
  echo "ERROR: xerces-c/$XERCES_HASH could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl xerces-c)"
  exit -1
fi
spack load -r boost/$BOOST_HASH 
if [ $? -ne 0 ] ; then
  echo "ERROR: boost/$BOOST_HASH could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl boost +thread+log+filesystem+date_time)"
  exit -1
fi
spack load -r xsd/$XSD_HASH
if [ $? -ne 0 ] ; then
  echo "ERROR: xsd/$XSD_HASH could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl xsd)"
  exit -1
fi

spack load -r cppunit/$CPPUNIT_HASH
if [ $? -ne 0 ] ; then
  echo "ERROR: cppunit/$CPPUNIT_HASH could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl cppunit)"
  exit -1
fi

# This will load openmpi and hdf5 and python3
spack load -r paraview/$PARAVIEW_HASH 
if [ $? -ne 0 ] ; then
  echo "ERROR: paraview/$PARAVIEW_HASH  could not be loaded. Please check that it has been installed via Spack. And check the hash used (spack find -vl paraview)"
  exit -1
fi

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
if [[ ! -d "$BOOSTDIR" ]] ; then
  echo "ERROR: BOOSTDIR could not be found. Please check that boost has been installed via Spack. And check the hash used (spack find -vl boost)"
  exit -1
fi
export XSDDIR=$(spack find -p xsd/$XSD_HASH  | grep xsd | sed "s|^.* ||g")
if [[ ! -d "$XSDDIR" ]] ; then
  echo "ERROR: XSDDIR could not be found. Please check that xsd has been installed via Spack. And check the hash used (spack find -vl xsd)"
  exit -1
fi
export XERCESSDIR=$(spack find -p xerces-c/$XERCES_HASH  | grep xerces-c | sed "s|^.* ||g")
if [[ ! -d "$XERCESSDIR" ]] ; then
  echo "ERROR: XERCESSDIR could not be found. Please check that xerces-c has been installed via Spack. And check the hash used (spack find -vl xerces-c)"
  exit -1
fi
export CPPUNITDIR=$(spack find -p cppunit/$CPPUNIT_HASH  | grep cppunit | sed "s|^.* ||g")
export HDF5DIR=$(spack find -p hdf5/$HDF5_HASH  | grep hdf5 | sed "s|^.* ||g")
if [[ ! -d "$HDF5DIR" ]] ; then
  echo "ERROR: HDF5DIR could not be found. Please check that xerces-c has been installed via Spack. And check the hash used (spack find -vl hdf5)"
  exit -1
fi
export SPACKMPIPATH=$(spack find -p openmpi/$OMPI_HASH | grep openmpi | sed "s|^.* ||g")
if [[ ! -d "$SPACKMPIPATH" ]] ; then
  echo "ERROR: SPACKMPIPATH could not be found. Please check that openmpi has been installed via Spack. And check the hash used (spack find -vl openmpi)"
  exit -1
fi
export CATALYSTDIR=$(spack find -p paraview/$PARAVIEW_HASH | grep paraview | sed "s|^.* ||g")
if [[ ! -d "$CATALYSTDIR" ]] ; then
  echo "ERROR: CATALYSTDIR could not be found. Please check that paraview has been installed via Spack. And check the hash used (spack find -vl paraview)"
  exit -1
fi


# N.B. As this path is very long it seems the #! format of Linux will not work, so make a soft link to the actual python below
# export PYTHONEXE=$(spack find -p python/$PYTHON_HASH  | grep cppunit | sed "s|^.* ||g")/bin/python
# if [[ ! -f $INSTALL_PREFIX/bin/python3 ]] ; then
#   ln -s $PYTHONEXE $INSTALL_PREFIX/bin/python3
#   chmod 744 $INSTALL_PREFIX/bin/python3
# fi



# compiling and installing Damaris
 echo -e "--- COMPILING DAMARIS -------------------------------------------------------------\n"

 

 cmake --verbose=1 $REPODIR -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PREFIX \
   -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
	-DBOOST_ROOT=$BOOSTDIR\
	-DXSD_ROOT=$XSDDIR \
	-DXercesC_ROOT=$XERCESSDIR \
	-DCMAKE_CXX_COMPILER=$SPACKMPIPATH/bin/mpicxx \
	-DCMAKE_C_COMPILER=$SPACKMPIPATH/bin/mpicc \
	-DCMAKE_Fortran_COMPILER=$SPACKMPIPATH/bin/mpifort  \
   -DENABLE_FORTRAN:BOOL=ON \
	-DENABLE_TESTS=OFF \
   -DENABLE_EXAMPLES=OFF -DCppUnit_ROOT=$CPPUNITDIR \
	-DBUILD_SHARED_LIBS=$SHLIBS_ON_OFF \
	-DENABLE_HDF5=ON -DHDF5_ROOT=$HDF5DIR \
   -DENABLE_CATALYST=ON -DParaView_DIR=$CATALYSTDIR \
   -DENABLE_VISIT=OFF
 

cd ..

echo -e ""

echo -e "--- FINISHED CONFIGURATION -----------------------------------------------------------\n"
echo -e "Now: "
echo -e "cd $BUILDDIR"
echo -e "make -j2"
echo -e "sudo make install"
 
