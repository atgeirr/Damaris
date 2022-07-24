#! /bin/bash
#OAR -l /nodes=1/core=16,walltime=00:05
#OAR -O stats_finalize_%jobid%.std
#OAR -E stats_finalize_%jobid%.err
#OAR -n stats_finalize

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: OAR run scripts for running 
# Damaris dask example 3dmesh_dask_stats on Grid5000


# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH

DASK_SCHEDULER_FILE=$1

python stats_get_results.py -f $DASK_SCHEDULER_FILE
