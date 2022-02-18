#! /bin/bash
# File:   opm_stack_build_vars.sh
# Author: Josh Bowden, Inria
# Date:   22/04/2021
# 
# This file is to be copied in to the  and sourced by singularity build definition files
# e.g.
# source /opmbuildstack/opm_stack_build_vars_ubu.sh
# 


# Enable the devtools environment 
# The number implies a GCC version
# DEVTOOLSET=devtoolset-9
# Installed using:
# yum -y install centos-release-scl
# yum -y install ${DEVTOOLSET}
#Enable the devtools environment 
# scl enable ${DEVTOOLSET} -- bash

# OMPBUILDSTACKDIR=/opmbuildstackdir

export GCC=/usr/bin/gcc
export GPP=/usr/bin/g++
export CC=$GCC
export CXX=$GPP
export CMAKE_C_COMPILER=$GCC
export CMAKE_CXX_COMPILER=$GPP
    
    
# The file is usefull to keep all variables consistent over multi-definition builds
 # Options: Debug | Release | MinSizeRel | RelWithDebInfo
CMAKE_BUILD_TYPE=Debug
INSTALL_PREFIX=/usr/local
# Directory where all libraries will be downloaded to and built from

# A lot og GCC specific flags
SUPERLU_BLAS="-L/usr/lib -lopenblas"
BLAS_LIB="-L/usr/lib -lopenblas"
LAPACK_LIB="-L/usr/lib -lopenblas"
CMAKE_CXX_FLAGS="-march=native"
CMAKE_C_FLAGS="-march=native"
export CFLAGS="-O2 -g -march=native"
export CXXFLAGS="-O2 -g -march=native"
# SuiteSparse
OPTIMIZATION="-O2 -g -march=native"
## Target CPU architectures example supported values are: 
## "none", "generic", "core","sandy-bridge", "ivy-bridge", "haswell", "broadwell", "skylake", "skylake-xeon", "kaby-lake", "cannonlake", "silvermont", "goldmont", "knl" (Knights Landing), "magny-cours", "bulldozer", "interlagos", "piledriver", "AMD 14h", "AMD 16h", "zen"   
TA="-DTARGET_ARCHITECTURE:STRING=auto"
OPENMPLIB="-fopenmp"
BUILD_SHARED="-DBUILD_SHARED_LIBS=TRUE"
BUILD_TESTING="-DBUILD_TESTING=FALSE"

# MPI paths
MPI_BINDIR=/usr/bin
MPI_LIBDIR=/usr/lib/x86_64-linux-gnu/openmpi/lib
MPI_INCDIR=/usr/lib/x86_64-linux-gnu/openmpi/include

# INSTALL_PREFIX=/usr
export PATH=$INSTALL_PREFIX/bin:$PATH
export PATH=${MPI_BINDIR}:$PATH
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$INSTALL_PREFIX/lib64:$MPI_LIBDIR:$LD_LIBRARY_PATH

# Tag values for git checkouts
SUPERLUTAG=v5.2.2
SUITESPARSETAG=v5.9.0
DUNETAG=v2.6.0
OPMTAG=release/2020.10
SCOTCH_TAG=v6.1.0
VC_TAG=1.4.1

# Build with this many cores
NUMCORES=4

# Update cmake on centos7
CMAKE_VERSION=3.16
CMAKE_VERSION_FULL=${CMAKE_VERSION}.3

# Damaris dependeciy library versions
PV_VERSION=5.8.0
PV_VERSION_SHORT=5.8
XERCES_VERSION=3.2.2
XSD_VERSION=4.0.0
# CPPUNIT_VERSION=1.12.1
# 1.8.12  (CentOS7 vwersion
HDFVERSION=1.10
HDFVERPOINT=.7


PYTHON_INCLUDE_DIR=/usr/include/python3.8
GL_INSTALL_PATH=/usr/lib/x86_64-linux-gnu
GL_HEADER_PATH=/usr
# Build damaris as shared library (N.B. static library build has problems)
SHLIBS_ON_OFF=ON
# Build examples
EXAMPLES_ON_OFF=ON
# Regenerate the xml model in C using XSD
REGEN_ON_OFF=ON
