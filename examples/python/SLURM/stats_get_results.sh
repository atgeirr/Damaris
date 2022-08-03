#! /bin/bash
#SBATCH --time=05:00
#SBATCH --mem-per-cpu=1GB
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=1
#SBATCH --ntasks-per-socket=1
#SBATCH --output=results_from_dask_%j.std
#SBATCH --job-name=results_from_dask
#SBATCH --account=try22_bowden
#SBATCH --partition=g100_usr_prod

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: OAR run scripts for running 
# Damaris dask example 3dmesh_dask_stats on SLURM based infrastructures


# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=$HOME/mypylib:$PYTHONPATH

# Passed in from sbatch using --export=DASK_SCHEDULER_FILE
# DASK_SCHEDULER_FILE=$1

python stats_get_results.py -f $DASK_SCHEDULER_FILE
