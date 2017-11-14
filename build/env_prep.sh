#!/bin/bash

# This script builds and installs all the prerequisites of Damaris.  
# Before running this script, please make sure that C/C++/Fortran
# compilers are installed on the target machine. All the needed libraries 
# are installed on $install_path, so please change this variable before 
# running, if you need to change it. 



install_visit=0
install_hdf5=0

mpi_impl=mpich  # either mpich or openmpi

export install_path=$HOME/local

export PATH=$install_path/bin:$PATH
export LD_LIBRARY_PATH=$install_path/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$install_path/lib64:$LD_LIBRARY_PATH

# create folders 
mkdir temp
cd temp
tempdir=$(pwd)


echo -e "--- COMPILING & INSTALLING CMAKE ---------------------------------------------------------------\n"
# Installing cmake
wget --no-check-certificate http://www.cmake.org/files/v3.0/cmake-3.0.1.tar.gz
tar -xzf cmake-3.0.1.tar.gz
cd cmake-3.0.1
./bootstrap --prefix=$install_path
make 
make install


echo -e "--- COMPILING & INSTALLING MPI LIBRARY -----------------------------------------------------------\n"
cd $tempdir

if [ $mpi_impl == mpich ]; then
  wget http://www.mpich.org/static/downloads/1.5rc3/mpich2-1.5rc3.tar.gz
  tar -xzf mpich2-1.5rc3.tar.gz
  cd mpich2-1.5rc3
  ./configure --prefix=$install_path --enable-shared --enable-romio --enable-fc 2>&1 | tee c.txt
  make 2>&1 | tee m.txt
  make install 2>&1 | tee mi.txt
else
  wget --no-check-certificate https://www.open-mpi.org/software/ompi/v2.1/downloads/openmpi-2.1.0.tar.gz
  tar -xzf openmpi-2.1.0.tar.gz
  cd openmpi-2.1.0
  ./configure --prefix=$install_path --enable-shared
  make
  make install
fi


echo -e "--- COMPILING & INSTALLING XERCESS ---------------------------------------------------------------\n"
# Installing xerces-c
cd $tempdir
wget --no-check-certificate https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.4.tar.gz
tar -xzf xerces-c-3.1.4.tar.gz
cd xerces-c-3.1.4
./configure --prefix=$install_path --disable-threads --disable-network
make
make install


echo -e "--- COMPILING & INSTALLING XSD ---------------------------------------------------------------\n"
# Installing xsd
cd $tempdir
wget   http://www.codesynthesis.com/download/xsd/4.0/xsd-4.0.0+dep.tar.bz2
tar -xjf xsd-4.0.0+dep.tar.bz2
cd  xsd-4.0.0+dep
make LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" CXXFLAGS="-I ${install_path}/include/"
make install_prefix=$install_path install LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" 
CXXFLAGS="-I ${install_path}/include/"


echo -e "--- COMPILING & INSTALLING BOOST ---------------------------------------------------------------\n"
# Installing boost
cd $tempdir
wget --no-check-certificate http://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.gz
tar -xzf boost_1_62_0.tar.gz
cd boost_1_62_0
./bootstrap.sh --prefix=$install_path --with-libraries=thread,log,date_time,program_options,filesystem,system
./b2 threading=multi,single
./b2 install


echo -e "--- COMPILING & INSTALLING CPPUNIT ---------------------------------------------------------------\n"
# Installing cppuint
cd $tempdir
wget --no-check-certificate  http://sourceforge.net/projects/cppunit/files/cppunit/1.12.1/cppunit-1.12.1.tar.gz
tar -xzf cppunit-1.12.1.tar.gz
cd cppunit-1.12.1
./configure --prefix=$install_path
make
make install


echo -e "--- COMPILING & INSTALLING HDF5 ---------------------------------------------------------------\n"
# Installing HDF5 1.10
if [ $install_hdf5 = 1 ]; then
    cd $tempdir
    wget --no-check-certificate https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.19.tar
    tar -xvf hdf5-1.8.19.tar
    cd hdf5-1.8.19
    ./configure --enable-parallel --prefix=$install_path
    make
    make install
fi

echo -e "--- COMPILING & INSTALLING VISIT -----------------------------------------------------------------\n"
# Installling Visit
if [ $install_visit = 1 ]; then
  cd $tempdir
  mkdir visit
  cd visit
  wget http://portal.nersc.gov/project/visit/releases/2.10.3/build_visit2_10_3
  chmod +x build_visit2_10_3
  ./build_visit2_10_3 --server-components-only --mesa --system-cmake --parallel --prefix $install_path/visit
fi


# echo -e "--- COMPILING & INSTALLING DAMARIS ---------------------------------------------------------------\n"
# compiling and installing Damaris
cd $tempdir
cd ..
cd ..
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=$install_path
make
make install

