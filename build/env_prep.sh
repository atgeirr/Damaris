#!/bin/bash

# This script builds and installs all the prerequisites of Damaris.  
# Before running this script, please make sure that C/C++/Fortran
# compilers are installed on the target machine. All the needed libraries 
# are installed on $install_path, so please change this variable before 
# running, if you need to change it. 

install_catalyst=1
install_visit=0
install_hdf5=0
make_jobs=8
mpi_impl=mpich  # either mpich or openmpi

export install_path=/home/hadi/local/

export PATH=$install_path/bin:$PATH
export LD_LIBRARY_PATH=$install_path/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$install_path/lib64:$LD_LIBRARY_PATH

# create folders 
mkdir temp
cd temp
tempdir=$(pwd)


echo -e "--- COMPILING & INSTALLING CMAKE ---------------------------------------------------------------\n"
# Installing cmake
wget --no-check-certificate http://www.cmake.org/files/v3.12/cmake-3.12.3.tar.gz
tar -xzf cmake-3.12.3.tar.gz
cd cmake-3.12.3
./bootstrap --prefix=$install_path
make -j$make_jobs
make install


echo -e "--- COMPILING & INSTALLING MPI LIBRARY -----------------------------------------------------------\n"
cd $tempdir

if [ $mpi_impl == mpich ]; then
  wget http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
  tar -xzf mpich-3.2.1.tar.gz
  cd mpich-3.2.1
  ./configure --prefix=$install_path --enable-shared --enable-romio --enable-fc 2>&1 | tee c.txt
  make -j$make_jobs 2>&1 | tee m.txt
  make install 2>&1 | tee mi.txt
else
  wget --no-check-certificate https://www.open-mpi.org/software/ompi/v2.1/downloads/openmpi-2.1.0.tar.gz
  tar -xzf openmpi-2.1.0.tar.gz
  cd openmpi-2.1.0
  ./configure --prefix=$install_path --enable-shared
  make -j$make_jobs
  make install
fi


echo -e "--- COMPILING & INSTALLING XERCESS ---------------------------------------------------------------\n"
# Installing xerces-c
cd $tempdir
wget --no-check-certificate https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.4.tar.gz
tar -xzf xerces-c-3.1.4.tar.gz
cd xerces-c-3.1.4
./configure --prefix=$install_path --disable-threads --disable-network
make -j$make_jobs
make install


echo -e "--- COMPILING & INSTALLING XSD ---------------------------------------------------------------\n"
# Installing xsd
cd $tempdir
wget http://www.codesynthesis.com/download/xsd/4.0/xsd-4.0.0+dep.tar.bz2
tar -xjf xsd-4.0.0+dep.tar.bz2
cd xsd-4.0.0+dep
make -j$make_jobs LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" CXXFLAGS="-I ${install_path}/include/"
make install_prefix=$install_path install LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" 
CXXFLAGS="-I ${install_path}/include/"



echo -e "--- COMPILING & INSTALLING BOOST ---------------------------------------------------------------\n"
# Installing boost
cd $tempdir
wget --no-check-certificate http://sourceforge.net/projects/boost/files/boost/1.67.0/boost_1_67_0.tar.gz
tar -xzf boost_1_67_0.tar.gz
cd boost_1_67_0
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
make -j$make_jobs
make install


echo -e "--- COMPILING & INSTALLING HDF5 ---------------------------------------------------------------\n"
# Installing HDF5 1.10
if [ $install_hdf5 = 1 ]; then
    cd $tempdir
    wget --no-check-certificate https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.20.tar
    tar -xvf hdf5-1.8.20.tar
    cd hdf5-1.8.20
    ./configure --enable-parallel --prefix=$install_path
    make -j$make_jobs
    make install
    hdf5_arg="-DENABLE_HDF5=ON -DHDF5_ROOT=$install_path"
fi

echo -e "--- COMPILING & INSTALLING VISIT -----------------------------------------------------------------\n"
# Installling Visit
if [ $install_visit = 1 ]; then
  cd $tempdir
  mkdir visit
  cd visit
  wget http://portal.nersc.gov/project/visit/releases/2.13.2/build_visit2_13_2
  chmod +x build_visit2_13_2
  ./build_visit2_13_2 --server-components-only --mesa --system-cmake --parallel --prefix $install_path/visit
  visit_arg="-DENABLE_VISIT=ON -DVisIt_ROOT=$install_path"
fi


echo -e "--- COMPILING & INSTALLING CATALYST -----------------------------------------------------------------\n"
# Installling Catalyst
if [ $install_catalyst = 1 ]; then
  cd $tempdir
  wget -O Catalyst-5.6.tar.gz "https://www.paraview.org/paraview-downloads/download.php?submit=Download&version=v5.6&type=catalyst&os=Sources&downloadFile=Catalyst-v5.6.0-RC2-Base-Enable-Python-Essentials-Extras-Rendering-Base.tar.gz"
  tar -xf Catalyst-5.6.tar.gz
  cd Catalyst-v5.6.0-RC2-Base-Enable-Python-Essentials-Extras-Rendering-Base
  mkdir catalyst-build
  cd catalyst-build
  ../cmake.sh .. -DCMAKE_INSTALL_PREFIX=$install_path
  make -j $make_jobs
  make install
fi


# echo -e "--- COMPILING & INSTALLING DAMARIS ---------------------------------------------------------------\n"
# compiling and installing Damaris
cd $tempdir
mkdir damaris-build
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
make -j $make_jobs
make install



