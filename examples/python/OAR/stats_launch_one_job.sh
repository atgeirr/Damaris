#!/bin/bash
#OAR -l /nodes=1/core=16,walltime=00:10
#OAR -O 3dmesh_stats_%jobid%.std
#OAR -E 3dmesh_stats_%jobid%.err
#OAR -n 3dmesh_stats

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: OAR run scripts for running 
# Damaris dask example stats_3dmesh_dask on Grid5000

# N.B. If 4 simulations are launched which each runs 4 iterations, 
#      and each adding a integer 1, 2, 3, or 4, distributed over the 4 simulations 
#      on each iteration, then: 
#      The mean of the first blocks will be 2.5 
#      and the variance for 4 iterations will be 1.333...


# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH

# sleep $(( RANDOM % 30 ))  # this was needed so multiple simulations did not launch at the same time
MY_VAL=$1

# We are assuming we are in the sub directory <INSTALL_PATH>/examples/damaris/python/OAR 
# and the executables are one directory up, so move there:
cd ..

#  Usage: stats_3dmesh_dask <stats_3dmesh_dask.xml> [-i I] [-d D] [-r] [-s S]
#  -i  I    I is the number of iterations of simulation to run
#  -v  V    V is the value to add to each array element (default = 5)
#  -d  D    D is the number of domains to split data into (must divide into WIDTH perfectly from XML file)
#  -s  S    S is integer time to sleep in seconds between iterations
mpirun -np 16 -mca pls_rsh_agent "/usr/bin/oarsh" -x PYTHONPATH=$PYTHONPATH  --hostfile $OAR_NODE_FILE ./stats_3dmesh_dask stats_3dmesh_dask.xml -i 4 -v $1 -d 4 -s 5

sleep 5
