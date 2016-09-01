#!/bin/bash

# This script builds and installs all the prerequisites of Damaris.  
# Before running this script, please make sure that C/C++/Fortran
# compilers are installed on the target machine. All the needed libraries 
# are installed on $install_path, so please change this variable before 
# running, if you need to change it. 


export install_path=/root/local

export PATH=$install_path/bin:$PATH
export LD_LIBRARY_PATH=$install_path/lib:$LD_LIBRARY_PATH

# create folders 
mkdir temp
cd temp
tempdir=$(pwd)


echo -e "--- COMPILING & INSTALLING CMAKE ---------------------------------------------------------------\n"
# Installing cmake
wget http://www.cmake.org/files/v3.0/cmake-3.0.1.tar.gz
tar -xzf cmake-3.0.1.tar.gz
cd cmake-3.0.1
./bootstrap --prefix=$install_path
make 
make install

echo -e "--- COMPILING & INSTALLING MPICH ---------------------------------------------------------------\n"
# Installing mpich
cd $tempdir
wget http://www.mpich.org/static/downloads/1.5rc3/mpich2-1.5rc3.tar.gz
tar -xzf mpich2-1.5rc3.tar.gz
cd mpich2-1.5rc3
./configure --prefix=$install_path --enable-shared --enable-romio --enable-fc 2>&1 | tee c.txt
make 2>&1 | tee m.txt
make install 2>&1 | tee mi.txt


echo -e "--- COMPILING & INSTALLING XERCESS ---------------------------------------------------------------\n"
# Installing xerces-c
cd $tempdir
wget http://mirrors.ircam.fr/pub/apache//xerces/c/3/sources/xerces-c-3.1.4.tar.gz
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
make install_prefix=$install_path install LDFLAGS="-L${install_path}/lib/" CFLAGS="-I ${install_path}/include" CXXFLAGS="-I ${install_path}/include/"


echo -e "--- COMPILING & INSTALLING BOOST ---------------------------------------------------------------\n"
# Installing boost
cd $tempdir
wget  http://sourceforge.net/projects/boost/files/boost/1.56.0/boost_1_56_0.tar.gz
tar -xzf boost_1_56_0.tar.gz
cd boost_1_56_0
./bootstrap.sh --prefix=$install_path --with-libraries=thread,log,date_time,program_options,filesystem,system
./b2
./b2 install

echo -e "--- COMPILING & INSTALLING CPPUNIT ---------------------------------------------------------------\n"
# Installing cppuint
cd $tempdir
wget  http://sourceforge.net/projects/cppunit/files/cppunit/1.12.1/cppunit-1.12.1.tar.gz
tar -xzf cppunit-1.12.1.tar.gz
cd cppunit-1.12.1
./configure --prefix=$install_path
make
make install

# echo -e "--- COMPILING & INSTALLING DAMARIS ---------------------------------------------------------------\n"
# compiling and installing Damaris
# cd $tempdir
# git clone https://scm.gforge.inria.fr/anonscm/git/damaris/damaris.git 
# cd damaris
# cd trunk
# cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=$install_path
# make
# make install


