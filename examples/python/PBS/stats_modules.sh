#! /bin/bash

# This file can be sourced by all jobs started via QSUB 

module load Boost/1.77.0-GCC-10.2.0-Python-3.8.6
module load OpenMPI/4.1.1-GCC-10.2.0

export INSTALL_PREFIX=$HOME/local
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$INSTALL_PREFIX/lib64:$LD_LIBRARY_PATH
export PATH=$INSTALL_PREFIX/bin:$PATH