#!/bin/bash

# This script builds and installs all the prerequisites of Damaris.  
# Before running this script, please make sure that C/C++/Fortran
# compilers are installed on the target machine. All the needed libraries 
# are installed on $install_path, so please change this variable before 
# running, if you need to change it. 

# prerequisites for debian
# sudo apt-get install gfortran gcc g++ git-core python-dev

v1CMAKE=1
# mpich or openmpi
v2MPI_IMPL=openmpi
v3XERCESS=1
v4XSD=1
v5BOOST=1
v6CPPUNIT=1
v7HDF5=1
v8VISIT=1
v9CATALYST=1
v10COMPILE=1

make_jobs=2

export install_path=/opt/damaris/local
export PATH=$install_path/bin:$PATH
export LD_LIBRARY_PATH=$install_path/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$install_path/lib64:$LD_LIBRARY_PATH

USAGE=$'Usage: env_prep.sh (all|compile|custom) \n [args:(cmake, mpi[mpich,openmpi,none], xercess, xsd, boost, cppunit, hdf5, visit, catalyst, compile)]'
EXAMPLE1="env_prep.sh all //(equiv env_prep.sh custom 1 mpich 1 1 1 1 1 1 1 1)"
EXAMPLE2="env_prep.sh compile //(equiv env_prep.sh custom 0 none 0 0 0 0 0 0 0 1)"
EXAMPLE3="env_prep.sh custom 1 mpich 1 1 1 1 1 1 1 0 //(all - compile)"
EXAMPLE4="env_prep.sh compile 1 1 1 //(compile with hdfs5 and visit and catalyst enabled)"
SERVICE=$1
ARGS=("${@:2}") # get remaining arguments as array

echo "options"
echo $ARGS

length=${#ARGS[@]}

hdf5_arg="-DENABLE_HDF5=OFF -DHDF5_ROOT=$install_path"
visit_arg="-DENABLE_VISIT=OFF -DVisIt_ROOT=$install_path"
catalyst_arg="-DENABLE_CATALYST=OFF -DParaView_DIR=$install_path"

case $SERVICE in
    (all)
        echo -e "--- COMPILING & INSTALLING DAMARIS dependencies and code ---------------------------------\n"
    ;;

    (compile)
        echo -e "--- COMPILING & INSTALLING DAMARIS code --------------------------------------------------\n"
        v1CMAKE=0
        v2MPI_IMPL=none #mpich or openmpi
        v3XERCESS=0
        v4XSD=0
        v5BOOST=0
        v6CPPUNIT=0
        v7HDF5=0
        v8VISIT=0
        v9CATALYST=0
        v10COMPILE=1
        if (( $length == 3 )); then
         if  [ ${ARGS[0]} == 1 ]; then
          hdf5_arg="-DENABLE_HDF5=ON -DHDF5_ROOT=$install_path"
         fi
         if  [ ${ARGS[1]} == 1 ]; then
          visit_arg="-DENABLE_VISIT=ON -DVisIt_ROOT=$install_path"
         fi
         if  [ ${ARGS[2]} == 1 ]; then
          catalyst_arg="-DENABLE_CATALYST=ON -DParaView_DIR=$install_path"
         fi
        fi
    ;;

    (custom)
        echo -e "--- COMPILING & INSTALLING DAMARIS code --------------------------------------------------\n"
        if [ $length == 10 ]; then
         v1CMAKE=${ARGS[0]}
         v2MPI_IMPL=${ARGS[1]}
         v3XERCESS=${ARGS[2]}
         v4XSD=${ARGS[3]}
         v5BOOST=${ARGS[4]}
         v6CPPUNIT=${ARGS[5]}
         v7HDF5=${ARGS[6]}
         v8VISIT=${ARGS[7]}
         v9CATALYST=${ARGS[8]}
         v10COMPILE=${ARGS[9]}
        else
          echo "Number of parameters is wrong"
          exit 1
        fi
    ;;

    (*)
        echo "Unknown service '${SERVICE}'. '$USAGE' as follows:."

        echo $EXAMPLE1
        echo $EXAMPLE2
        echo $EXAMPLE3
        echo $EXAMPLE4
        exit 1
    ;;
esac

# create folders 
mkdir -p temp
cd temp
tempdir=$(pwd)

echo "START"

if (( $v1CMAKE == 1 )); then
  echo -e "--- COMPILING & INSTALLING CMAKE ---------------------------------------------------------------\n"
  # Installing cmake
  wget --no-check-certificate http://www.cmake.org/files/v3.12/cmake-3.12.3.tar.gz
  tar -xzf cmake-3.12.3.tar.gz
  cd cmake-3.12.3
  ./bootstrap --prefix=$install_path
  make -j1
  make install
fi

if [ $v2MPI_IMPL == mpich ]; then
  echo -e "--- COMPILING & INSTALLING MPICH LIBRARY -----------------------------------------------------------\n"
  cd $tempdir
  wget http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
  tar -xzf mpich-3.2.1.tar.gz
  cd mpich-3.2.1
  ./configure --prefix=$install_path --enable-shared --enable-romio --enable-fc 2>&1 | tee c.txt
  make -j$make_jobs 2>&1 | tee m.txt
  make install 2>&1 | tee mi.txt
elif [ $v2MPI_IMPL == openmpi ]; then
  echo -e "--- COMPILING & INSTALLING OPENMPI LIBRARY -----------------------------------------------------------\n"
  cd $tempdir
  wget --no-check-certificate https://www.open-mpi.org/software/ompi/v2.1/downloads/openmpi-2.1.0.tar.gz
  tar -xzf openmpi-2.1.0.tar.gz
  cd openmpi-2.1.0
  ./configure --prefix=$install_path --enable-shared
  make -j$make_jobs
  make install
fi


if (( $v3XERCESS == 1 )); then
 echo -e "--- COMPILING & INSTALLING XERCESS ---------------------------------------------------------------\n"
 # Installing xerces-c
 cd $tempdir
 wget --no-check-certificate https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.2.2.tar.gz
tar -xzf xerces-c-3.2.2.tar.gz
cd xerces-c-3.2.2
./configure --prefix=$install_path --disable-threads --disable-network
make -j$make_jobs
make install
fi

if (( $v4XSD == 1 )); then
 echo -e "--- COMPILING & INSTALLING XSD ---------------------------------------------------------------\n"
 # Installing xsd
 cd $tempdir
 wget http://www.codesynthesis.com/download/xsd/4.0/xsd-4.0.0+dep.tar.bz2
 tar -xjf xsd-4.0.0+dep.tar.bz2
 cd xsd-4.0.0+dep
 make -j$make_jobs LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" CXXFLAGS="-I ${install_path}/include/"
 make install_prefix=$install_path install LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" CXXFLAGS="-I ${install_path}/include/"
fi


if (( $v5BOOST == 1 )); then
 echo -e "--- COMPILING & INSTALLING BOOST ---------------------------------------------------------------\n"
 # Installing boost
 cd $tempdir
 wget --no-check-certificate http://sourceforge.net/projects/boost/files/boost/1.67.0/boost_1_67_0.tar.gz
 tar -xzf boost_1_67_0.tar.gz
 cd boost_1_67_0
 ./bootstrap.sh --prefix=$install_path --with-libraries=thread,log,date_time,program_options,filesystem,system
 # ./b2 threading=multi,single # seem like both of these together is not supported
 ./b2 threading=multi
 ./b2 install
fi


if (( $v6CPPUNIT == 1 )); then
 echo -e "--- COMPILING & INSTALLING CPPUNIT ---------------------------------------------------------------\n"
 # Installing cppuint
 cd $tempdir
 wget --no-check-certificate  http://sourceforge.net/projects/cppunit/files/cppunit/1.12.1/cppunit-1.12.1.tar.gz
 tar -xzf cppunit-1.12.1.tar.gz
 cd cppunit-1.12.1
 ./configure --prefix=$install_path
 make -j$make_jobs
 make install
fi

 # Installing HDF5 1.10
if (( $v7HDF5 == 1 )); then
    echo -e "--- COMPILING & INSTALLING HDF5 ---------------------------------------------------------------\n"
    cd $tempdir
    # wget --no-check-certificate https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.20.tar
    wget --no-check-certificate https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8/hdf5-1.8.20/src/hdf5-1.8.20.tar.gz
    tar -xvf hdf5-1.8.20.tar.gz
    cd hdf5-1.8.20
    ./configure --enable-parallel --prefix=$install_path
    make -j$make_jobs
    make install
    hdf5_arg="-DENABLE_HDF5=ON -DHDF5_ROOT=$install_path"
fi


if (( $MED == 1 )); then
    echo -e "--- COMPILING & INSTALLING MED (requires HDF) -------------------------------------------------\n"
    cd $tempdir
    wget https://files.salome-platform.org/Salome/other/med-4.1.0.tar.gz
    tar -xzf med-4.1.0.tar.gz
    # Options: Debug | Release | MinSizeRel | RelWithDebInfo
    mkdir ../medbuild
    
    export CXXFLAGS=-O2
    export CFLAGS=-O2
    cd ../medbuild
    ../med-4.1.0/configure --prefix=$install_path \
    --disable-fortran \
    --with-hdf5=$HDF5_ROOT \
    --disable-python \
    --with-med_int=int \
    --with-int64=long \
    --with-mpi=$MPI_ROOT
    
    
#     --with-f90=$MPI_ROOT/bin/mpif90 \
#     FFLAGS="-fdefault-integer-8"  \
#     FCFLAGS="-fdefault-integer-8" \
#     
#     CMAKE_BUILD_TYPE=Release
#     mkdir ../medbuild
#     cd ../medbuild
#     cmake ../med-4.1.0 -Wno-dev -DCMAKE_INSTALL_PREFIX:PATH=$install_path \
#     -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
#     -DMED_MEDINT_TYPE=int \
#     -DCMAKE_CXX_FLAGS:STRING=-DMED_API_23=1 \
#     -DCMAKE_C_FLAGS:STRING=-DMED_API_23=1 \
#     -DMED_API_23=1 \
#     -DMEDFILE_USE_MPI=YES \
#     -DMEDFILE_BUILD_TESTS=OFF \
#     -DMEDFILE_BUILD_PYTHON=OFF \
#     -DMEDFILE_INSTALL_DOC=OFF \
#     -DMEDFILE_BUILD_SHARED_LIBS=off \
#     -DMEDFILE_BUILD_STATIC_LIBS=ON \
#     -DCMAKE_Fortran_COMPILER=$MPI_ROOT/bin/mpif90 \
#     -DHDF5_ROOT_DIR=$HDF5_ROOT \
#     -DMPI_ROOT_DIR=$MPI_ROOT 

fi
#prerequisites
# sudo apt-get install subversion xutils-dev libxt-dev python-libxml2 zlib1g-dev aptitude
# sudo aptitude install freeglut3-dev libgl1-mesa-dev libdrm-dev -> n then y 
# ! still some issue compiling IceT 1.0 ! -> with cmake 2.6.4 it worked compiling IceT
# But Visit needs cmake 3.8 and higher, so we move to latest cmake


# meson builddir -Dosmesa=gallium -Dgallium-drivers=swrast -Ddri-drivers=[] -Dvulkan-drivers=[] -Dprefix=$PWD/builddir/install
# ninja -C builddir install

#Just run: ./env_prep.sh custom 0 none 0 0 0 0 0 1 0 0
# Installling Visit
if (( $v8VISIT == 1 )); then
  echo -e "--- COMPILING & INSTALLING VISIT -----------------------------------------------------------------\n"
  cd $tempdir
  mkdir -p visit
  cd visit
  # wget http://portal.nersc.gov/project/visit/releases/2.13.2/build_visit2_13_2
  wget https://github.com/visit-dav/visit/releases/download/v3.1.1/build_visit3_1_1
  chmod +x build_visit2_13_2
  #./build_visit2_13_2 --server-components-only --mesa --system-cmake --parallel --prefix $install_path/visit
PAR_COMPILER=$install_path/bin/mpicc \
PAR_COMPILER_CXX=$install_path/bin/mpicxx \
PAR_INCLUDE=-I$install_path/include \
PAR_LIBS=-lmpi \
./build_visit3_1_1 --server-components-only --mesagl --system-cmake --parallel --prefix $install_path/visit
  visit_arg="-DENABLE_VISIT=ON -DVisIt_ROOT=$install_path"
fi

#prerequisites
#sudo su; apt-get install aptitude
#aptitude install freeglut3-dev libgl1-mesa-dev libdrm-dev

#Just run: ./env_prep.sh custom 0 none 0 0 0 0 0 0 1 0
# Installling Catalyst
if (( $v9CATALYST == 1 )); then
  echo -e "--- COMPILING & INSTALLING CATALYST -----------------------------------------------------------------\n"
  cd $tempdir
  wget -O Catalyst-5.6.tar.gz "https://www.paraview.org/paraview-downloads/download.php?submit=Download&version=v5.6&type=catalyst&os=Sources&downloadFile=Catalyst-v5.6.0-RC2-Base-Enable-Python-Essentials-Extras-Rendering-Base.tar.gz"
  tar -xvf Catalyst-5.6.tar.gz
  cd Catalyst-v5.6.0-RC2-Base-Enable-Python-Essentials-Extras-Rendering-Base
  mkdir -p catalyst-build
  cd catalyst-build
  ../cmake.sh .. -DCMAKE_INSTALL_PREFIX=$install_path
  make -j $make_jobs
  make install
fi


# compiling and installing Damaris
if (( $v10COMPILE == 1 )); then
 echo -e "--- COMPILING & INSTALLING DAMARIS ---------------------------------------------------------------\n"

 cd $tempdir
 mkdir -p damaris-build
 cd damaris-build
 cmake ../../.. -DCMAKE_INSTALL_PREFIX:PATH=$install_path \
	-DBOOST_ROOT=$install_path \
	-DXSD_ROOT=$install_path \
	-DXercesC_ROOT=$install_path \
	-DCppUnit_ROOT=$install_path \
	-DCMAKE_CXX_COMPILER=mpicxx \
	-DCMAKE_C_COMPILER=mpicc \
	-DENABLE_TESTS=ON \
	-DENABLE_EXAMPLES=ON \
	-DBUILD_SHARED_LIBS=OFF \
	$visit_arg \
	$hdf5_arg
  $catalyst_arg
 make -j $make_jobs
 make install
fi


