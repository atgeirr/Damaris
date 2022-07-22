#!/bin/bash
#OAR -l /nodes=1/core=16,walltime=00:10
#OAR -O 3dmesh_stats_%jobid%.std
#OAR -E 3dmesh_stats_%jobid%.err
#OAR -n 3dmesh_stats

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: OAR run scripts for running 
# Damaris dask example 3dmesh_dask_stats on Grid5000


# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH

sleep $(( RANDOM % 30 ))
MY_VAL=$1
# Assumes 4 Damaris clients and 2 Damaris server cores as per the xml file
mpirun -np 16 -mca pls_rsh_agent "/usr/bin/oarsh" -x PYTHONPATH=$PYTHONPATH  --hostfile $OAR_NODE_FILE ./3dmesh_dask_stats 3dmesh_dask_stats.xml -v $1 -i 4 -d 4 -s 5

sleep 5
