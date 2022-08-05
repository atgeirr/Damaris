#! /bin/bash
#PBS -l walltime=00:15:00
#PBS -l select=1
#PBS -j oe
#PBS -m n
#PBS -o results_from_dask.std
#PBS -N results_from_dask


# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: PBSpro run scripts for running 
# Damaris dask example 3dmesh_dask_stats on PBSpro based infrastructures

cd $PBS_O_WORKDIR

. stats_modules.sh


# To be passed in on qsub command line using -v
echo "PYTHONPATH=$PYTHONPATH "
echo "DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE"

# Set path to Damaris Python module (for getting server MPI communicator)
# export PYTHONPATH=$HOME/mypylib:$PYTHONPATH

# Passed in from qsub using -v
# DASK_SCHEDULER_FILE=$1

python stats_get_results.py -f $DASK_SCHEDULER_FILE
