#!/bin/bash
#PBS -l walltime=00:30:00
#PBS -l select=1
#PBS -j oe
#PBS -m n
#PBS -o scheduler_job.o
#PBS -N scheduler


# Author: Josh Bowden, INRIA
# Date: 29/07/2022
# Description: Launch Dask scheduler
# Damaris Dask example 3dmesh_dask_stats on a SLURM based cluster
echo "We are now running the stats_launch_scheduler.sh script..."

cd $PBS_O_WORKDIR
. stats_modules.sh

echo $(python -m site --user-site)
 
# To be passed in on qsub command line using -v
echo "PYTHONPATH         =$PYTHONPATH "
echo "DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE"
echo "PATH               =$PATH"


# number of OpenMP threads
# export OMP_NUM_THREADS=$NCPUS 
# OpenMP binding
export OMP_PLACES=sockets


## This is for using dask-mpi
# mpirun -np 1 --oversubscribe --mca opal_warn_on_missing_libcuda 0   dask-mpi --scheduler-file $DASK_SCHEDULER_FILE --nthreads $SLURM_CPUS_PER_TASK   &
# --worker-class=distributed.Worker Nanny - use this if NWorkers are getting killed
# --memory-limit=0.8
# mpirun -np 1 --oversubscribe --mca opal_warn_on_missing_libcuda 0   dask-mpi --scheduler-file $DASK_SCHEDULER_FILE --no-scheduler --worker-class=distributed.Worker --nthreads $SLURM_CPUS_PER_TASK  --memory-limit=0.8 &


echo "INFO $0 : Launching scheduler...  --scheduler-file = $DASK_SCHEDULER_FILE"
dask-scheduler --scheduler-file $DASK_SCHEDULER_FILE &
sleep 5

echo "INFO $0 : Launching worker...  --scheduler-file = $DASK_SCHEDULER_FILE  --nthreads $SLURM_CPUS_PER_TASK"
# This should block until the scheduler is shutdown by a client.shutdown()
dask-worker --scheduler-file $DASK_SCHEDULER_FILE --nthreads $NCPUS 









