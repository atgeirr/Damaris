The **offical documentation** for Damaris is available here: https://project.inria.fr/damaris/documentation/ 

The following procedures may be out of date, however provides some extra information
for installing Damaris using Spack and in a container via Docker.

# Table of Contents
1. [Dependencies](#dependencies)
2. [Building Damaris using Spack](#building-damaris-using-spack)
3. [Building a Docker image with Damaris](#building-a-docker-image-with-damaris)
4. [Building Damaris using the env_prep.sh script](#building-damaris-using-the-env_prepsh-script)
5. [Building dependencies and Damaris manually](#building-dependencies-and-damaris-manually)


# Installing Damaris
==================

This file documents different ways from installing Damaris along
with its dependencies. If you want to install Damaris in a docker
please refer to the corresponding section. If you want to install
Damaris in another environment, we highly recommand using Spack
(see corresponding section), which can be used to install all the
necessary dependencies. Other sections document alternate ways to
build Damaris and its dependencies.


## Dependencies
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
  * (optional) ParaView (version 5.8 is recomended)


## Building Damaris using Spack
-------------------------------

Spack (https://spack.io/) is a powerful package manager that is often
used on clusters and supercomputers. Spack can be installed by following
the instructions at https://spack.readthedocs.io.

Damaris now has a Spack packages file that is part of the Spack git repository.
So, now it is easy enough to install Damaris. To see the options enabled through Spack

    spack info damaris
    
To install the base damaris with HDF5 support (no visulization support)
  
    spack install damaris+hdf5+fortran+examples

The Spack version of the Damaris 'package.py' file may be out of date. An updated Damaris Spack 'package.py' file is available from the [gitlab repository](https://gitlab.inria.fr/Damaris/damaris/-/tree/master/build/spack/repo/packages/damaris) and can be used by setting up a custom Spack repository by adding the path to the `build/spack/repo` directory to the file `~/.spack/repos.yaml`. 

```bash
$ cat ~/.spack/repos.yaml
repos:
  - https://gitlab.inria.fr/Damaris/damaris/-/tree/master/build/spack/repo

```

Some detalied instructions on using Spack to install Damaris with visulisation support are being developed and are [available here](https://gitlab.inria.fr/Damaris/damaris-development/-/blob/master/docs/build/damaris_catalyst_with_spack/damaris_catalyst_spack_install.md)
    
-------------------------------

Prior to having a full Spack package file, Damaris could be installed using Spack built dependencies using the following procedure:

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
command. To build the examples add -DENABLE_EXAMPLES=ON to the cmake command.


## Building a Docker image with Damaris
---------------------------------------
There are Dockerfiles in the ```build/docker``` directory based on Ubuntu (apt-get)
or Centos 8 (Yum / dnf) that can be used to build containers with Damaris 
installed. Both use the pre-built Damaris prerequisite libraries (Boost,
XSD and xerces-c), OpenMPI, HDF5 and CppUnit of thoses distributions. 

The *docker build* command line uses:  
```bash
--build-arg INPUT_repo=damaris  
```
 to specify the Damaris gitlab repository, and  
```bash
--build-arg INPUT_damaris_ver=${TAG}  
```
where TAG is defined as the repository branch to build (e.g. v1.5.0 or master etc.)

### To build the damaris repo (no password needed):
```bash
 sudo TAG=v1.5.0 DOCKER_BUILDKIT=1 docker build -t \
            registry.gitlab.inria.fr/damaris/damaris-ubu20:${TAG} \
            --build-arg INPUT_damaris_ver=${TAG} \
            --build-arg INPUT_repo=damaris \
                -f ./Dockerfile.ubuntu20 .
```
The Dockerfiles in ```build/docker``` are used in the Gitlab enabled continuous integration system
so please do not change them without testing carefully. As part of the CI system, the containers are
pushed to Gitlab container registry *[registry.gitlab.inria.fr](https://gitlab.inria.fr/Damaris/damaris/container_registry)*.  
  
### Log in to the gitlab repository 
 N.B. You may need a ~/.docker/config.json file configured with an access token
```bash
 sudo docker login registry.gitlab.inria.fr

# pull the latest version (or another tagged version)
docker pull registry.gitlab.inria.fr/damaris/damaris:latest"
```

### To test the created docker image:
```bash
# Get a shell into the container
sudo docker run --rm -it registry.gitlab.inria.fr/damaris/damaris:<branch> /bin/bash
 
# Run unit tests:
<container prompt> cd /home/docker/local/examples
<container prompt> ctest
 
# run and example:
<container prompt> mpirun -np 4  /home/docker/local/examples/storage/3dmesh /home/docker/local/examples/storage/3dmesh.xml [-v] [-r]
 
# Should find HDF5 output
<container prompt> ls *.h5
<container prompt> h5dump *.h5
```

## Building a Singularity image containing Damaris
---------------------------------------
### Introduction
On systems you do not have admin/sudo rights you may have access to the Singularity container system.
If available, Singularity will let you run containers, however the creation of the container will need to 
be done on a system where you do have admin/sudo rights. You then need to copy the image to the system 
where you only have normal user rights and you will then be able to run the container.

### Build Singularity container from a Singularity definition file 
There are Singularity definition files in the ```build/singularity``` directory that are based on Ubuntu (apt-get)
or Centos 8 (Yum / dnf) that can be used to build containers with Damaris installed. Similar to the Dockerfiles,
both use the pre-built Damaris prerequisite libraries (Boost,XSD and xerces-c), OpenMPI, HDF5 and CppUnit of thoses distributions. 

1/ Build the container on your system with sudo access
```
    # add --sandbox if you want to mount and modify the container
    sudo singularity build  damaris_centos8 damaris_centos8.def

    # To enter the container in writable mode
    sudo singularity shell --writable damaris_centos8

    # 
```
2/ Copy to your machine (e.g. HPC cluster)
```
    scp damaris_centos8 <user>@headnode.system.xyz:~/
```
3/ From the machine, run the example code (using PBS to obtain an interactive job first ) 
```
qsub -I <arguments for qsub interactive job>
# ... wait for job resources to be provided ...

# Load modules needed
module load singularity openmpi
cd ~
# run the example code that is inside the container
mpirun -np 4 singularity exec ./damaris_centos8  /usr/local/examples/storage/3dmesh /usr/local/examples/storage/3dmesh.xml [-v] [-r]

```

### Build Singularity container from a Docker image 
Singularity can pull and build containers from Docker reporitories.

  
## Building Damaris using the env_prep.sh script
------------------------------------------------

Although this script has been written primarily to be called when
building a Docker image, it can be used to build all the dependencies
as well as Damaris on a normal machine.

Simply edit the script as instructed in the previous section and call:

    ./env_prep.sh

This will install everything from MPI to Damaris itself.


## Building dependencies and Damaris manually
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

Building VisIt's LibSim is challening enough that we won't go into detail
here. In most cases, it is sufficient to download the build_visitX_Y_Z
script (recommanded version is 2.13.2) from
https://wci.llnl.gov/simulation/computer-codes/visit/source
and call the following command:

./build_visit2_10_3 --server-components-only --mesa --system-cmake \
	--system-python --parallel --prefix $HOME/local

Remove --system-cmake if you don't have cmake installed on your platform.
Remove --system-python if you don't have python (including development
libraries) installed on your platform.
Change $HOME/local if you want it installed in another directory.

Once the procedure completes, VisIt is installed and you can enable
it using Damaris' cmake flags:

    -DENABLE_VISIT=ON \
    -DVisIt_ROOT=$HOME/local/visit

Note that if you install Damaris and VisIt on a Docker image or directly
using the env_prep.sh script, you can enable VisIt simply by toggling
install_visit=1 in env_prep.sh.


7) Building with Catalyst support
---------------------------------

At the time of writing this, spack fails to install its catalyst package. 
Hence we will need to build it manually.

First, download one of the Catalyst archives here:
https://www.paraview.org/download/
(this tutorial has been tested with 
Catalyst-v5.6.0-RC1-Base-Enable-Python-Essentials-Extras-Rendering-Base)

Run the following command (adapting to the version of Catalyst you wish
to install):

    tar xvf Catalyst-<selected-version>.tar.gz
    cd Catalyst-<selected-version>
    mkdir build
    cd build
    ../cmake.sh .. -DCMAKE_INSTALL_PREFIX=$HOME/local
    make
    make install

You can of course change the prefix where Catalyst will be installed.

Now you can add the following cmake flags when building Damaris to enable
Catalyst:

    -DENABLE_CATALYST=ON \
    -DParaView_DIR=$HOME/local/lib/cmake/paraview-5.6

Again, adapt the second line if you installed Catalyst somewhere else.
