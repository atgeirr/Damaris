#!/bin/bash
#OAR -l /nodes=1/core=16,walltime=01:00
#OAR -O scheduler_%jobid%.std
#OAR -E scheduler_%jobid%.err
#OAR -n scheduler

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: Launch Dask scheduler
# Damaris Dask example 3dmesh_dask_stats on Grid5000
echo "We are now running the stats_launch_scheduler.sh script..."

DASK_SCHEDULER_FILE=$1
# DASK_SCHEDULER_FILE=/home/jbowden/dask_file.json

dask-scheduler --scheduler-file $DASK_SCHEDULER_FILE &

# This should block until the scheduler is shutdown by a client.shutdown()
dask-worker --scheduler-file $DASK_SCHEDULER_FILE --nthreads 14





