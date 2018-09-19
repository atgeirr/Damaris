Installing Damaris
==================

This file documents different ways from installing Damaris along
with its dependencies. If you want to install Damaris in a docker
please refer to the corresponding section. If you want to install
Damaris in another environment, we highly recommand using Spack
(see corresponding section), which can be used to install all the
necessary dependencies. Other sections document alternate ways to
build Damaris and its dependencies.


1) Dependencies
---------------

Damaris requires the following dependencies.
  * CMake (version 3.11 or greater)
  * MPI compilers (e.g. MPICH, OpenMPI,...)
  * Boost (version 1.67 or greater)
  * Xerces-C (version 3.1.4 or greater)
  * XSD (version 4.0 or greater)
  * (optional) CppUnit (version 1.12.1 or greater)
  * (optional) HDF5 (version 1.8.20 or greater)
  * (optional) VisIt (version 2.10.3 or greater)
  * (optional) ParaView (version 5.6 or greater)


2) Building Damaris using Spack
-------------------------------

Spack (https://spack.io/) is a powerful package manager that is often
used on clusters and supercomputers. Spack can be installed by following
the instructions at https://spack.readthedocs.io.

Once spack is installed, you need to install cmake, boost, Xerces-C, and
XSD (and possibly MPI if you don't have it):

    spack install cmake
    spack install mpi
    spack install boost
    spack install xercesc
    spack install xsd

Optionally, install CppUnit and HDF5:

    spack install cppunit
    spack install hdf5

Load all the packages you just installed by calling spack load, e.g.

    spack load cmake
    spack load mpi
    ...

From the build directory of the Damaris source tree, call the following
commands:

    cmake ../.. -DCMAKE_INSTALL_PREFIX=$HOME/local \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_C_COMPILER=mpicc \
	-DCMAKE_CXX_COMPILER=mpicxx
    make
    make install

If you want to enable CppUnit tests, add -DENABLE_TESTS=ON to the cmake
command. If you want to enable HDF5, add -DENABLE_HDF5=ON to the cmake
command.


3) Building a Docker image with Damaris
---------------------------------------

Go to the build folder inside the Damaris source tree. You will find
a number of scripts available, in particular a env_prep.sh script
that can be used to download, compile, and install all the dependencies
as well as Damaris. This script will be used during the process of
building the Docker image.

Edit this script as follows:
  * Change the install_path variable to the desired location. 
  * Change install_visit to 1 if you want VisIt support.
  * Change install_hdf5 to 1 if you want HDF5 support.
  * Change the desired MPI library (options are mpich or openmpi).
  * Change the number of jobs Make can run concurrently with make_jobs=X

Call the following command to build a Docker image based on a SUSE
operating system:

    docker build -f Dockerfile.suse -t damaris-suse:1.0 .

Call the following command to build a Docker image based on a Debian
operating system:

    docker build -f Dockerfile.debian -t damaris-debian:1.0

This command creates an image, namely damaris-suse or damaris-debian,
with 1.0 as its tag, and runs the env_prep.sh scrip inside it. 
Do no forget to update env_prep.sh script beforehand. 


4) Building Damaris using the env_prep.sh script
------------------------------------------------

Although this script has been written primarily to be called when
building a Docker image, it can be used to build all the dependencies
as well as Damaris on a normal machine.

Simply edit the script as instructed in the previous section and call:

    ./env_prep.sh

This will install everything from MPI to Damaris itself.


5) Building dependencies and Damaris manually
---------------------------------------------

This section can help building Damaris and its dependencies manually.
Note that we highly recommend to use an alternative method such as
using the Dockefile for a docker image or using Spack to install on
any other environment.

For simplicity reasons, we will install everything in the $HOME/local
folder.

If cmake is not installed on your machine, download it at
https://cmake.org/download/, uncompress it and execute the following:

    ./bootstrap --prefix=$HOME/local
    make
    make install

If MPI is not installed on your machine, download MPICH it at 
http://www.mpich.org/downloads/, uncompress it and execute the
following commands:

    ./configure --prefix=$HOME/local \
                --enable-shared \
                --enable-romio \
		--enable-fc \
    make 
    make install

Add cmake's and MPICH's binary path to your PATH environment variable:

    export PATH=$HOME/local/bin:$PATH

Install Xerces-C by downloading it from http://xerces.apache.org, by
decompressing it and by running the following commands:

    ./configure --prefix=$HOME/local \
                --disable-threads --disable-network
    make
    make install

Install XSD by downloading it at
https://www.codesynthesis.com/download/xsd/4.0/xsd-4.0.0+dep.tar.bz2
Decompress it and run the following commands:

    make LDFLAGS="-L$HOME/local/lib/" CFLAGS="-I$HOME/local/include" \ 
         CXXFLAGS="-I$HOME/local/include"
    make install_prefix=$HOME/local install LDFLAGS="-L$HOME/local/lib"\ 
         CFLAGS="-I$HOME/local/include" CXXFLAGS="-I$HOME/local/include"

Install boost by downloading it from https://www.boost.org/, 
decompressing it and running the following commands:

    ./bootstrap.sh --prefix=$HOME/local \
        --with-libraries=thread,log,date_time,program_options,filesystem,system
    ./b2
    ./b2 install

Optionally, download and install CppUnit by downloading it from
https://sourceforge.net/projects/cppunit/files/cppunit/1.12.1/cppunit-1.12.1.tar.gz
Uncompress it, and call the following commands:

    ./configure --prefix=$HOME/local
    make
    make install

Optionally, download and install HDF5 by downloading it from
https://support.hdfgroup.org/HDF5/, uncompressing it, and calling
the following commands:

    ./configure --enable-parallel --prefix=$HOME/local
    make
    make install

Installing VisIt and Catalyst is more complicated and will be treated in
another section.

Once all the depencendies have been properly installed, execute the
following commands from the build directory of the Damaris source tree:

    mkdir temp
    cd temp
    cmake ../.. -DCMAKE_INSTALL_PREFIX=$HOME/local \
	-DBUILD_SHARED_LIBS=ON \
	-DBOOST_ROOT=$HOME/local \
	-DXERCESC_ROOT=$HOME/local \
	-DXSD_ROOT=$HOME/local \
	-DCMAKE_C_COMPILER=$HOME/local/bin/mpicc \
	-DCMAKE_CXX_COMPILER=$HOME/local/bin/mpicxx
    make
    make install

Note that if you want CppUnit test to be enabled, you will need add the
following flags to the cmake command:
 
    -DENABLE_TESTS=ON -DCppUnit_ROOT=$HOME/local

If you want HDF5 support, you will need to add the following flags to
the cmake command:

    -DENABLE_HDF5=ON -DHDF5_ROOT=$HOME/local

You can now check in $HOME/local that it contains the libraries in
the lib folder and the headers in the include folder.


6) Building with VisIt support
------------------------------

Building VisIt is challening enough that we won't go into detail here.

7) Building with Catalyst support
---------------------------------

TODO
