#!/bin/bash
#SBATCH --time=15:00
#SBATCH --mem-per-cpu=2GB
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --ntasks-per-socket=1
#SBATCH --cpus-per-task=24
#SBATCH --output=scheduler_%j.std
#SBATCH --job-name=scheduler
#SBATCH --account=try22_bowden
#SBATCH --partition=g100_usr_prod

# Author: Josh Bowden, INRIA
# Date: 29/07/2022
# Description: Launch Dask scheduler
# Damaris Dask example 3dmesh_dask_stats on a SLURM based cluster
echo "We are now running the stats_launch_scheduler.sh script..."

module list


DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE
echo  "$0 : scheduler file: $DASK_SCHEDULER_FILE)"

# number of OpenMP threads
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK 
# OpenMP binding
export OMP_PLACES=sockets


# mpirun -np 1 --oversubscribe --mca opal_warn_on_missing_libcuda 0   dask-mpi --scheduler-file $DASK_SCHEDULER_FILE --nthreads $SLURM_CPUS_PER_TASK   &
# --worker-class=distributed.Worker Nanny - use this if NWorkers are getting killed
# --memory-limit=0.8
# mpirun -np 1 --oversubscribe --mca opal_warn_on_missing_libcuda 0   dask-mpi --scheduler-file $DASK_SCHEDULER_FILE --no-scheduler --worker-class=distributed.Worker --nthreads $SLURM_CPUS_PER_TASK  --memory-limit=0.8 &
# DASK_SCHEDULER_FILE=$HOME/dask_file.json

echo "INFO $0 : Launching scheduler...  --scheduler-file = $DASK_SCHEDULER_FILE"
dask-scheduler --scheduler-file $DASK_SCHEDULER_FILE &
sleep 5

echo "INFO $0 : Launching worker...  --scheduler-file = $DASK_SCHEDULER_FILE  --nthreads $SLURM_CPUS_PER_TASK"
# This should block until the scheduler is shutdown by a client.shutdown()
dask-worker --scheduler-file $DASK_SCHEDULER_FILE --nthreads $SLURM_CPUS_PER_TASK 









