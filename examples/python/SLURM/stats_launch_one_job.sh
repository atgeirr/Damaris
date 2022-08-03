#!/bin/bash
#SBATCH --time=05:00
#SBATCH --mem-per-cpu=1GB
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=48
#SBATCH --cpus-per-task=1
#SBATCH --output=3dmesh_stats_%j.std
#SBATCH --job-name=3dmesh_stats
#SBATCH --account=try22_bowden
#SBATCH --partition=g100_usr_prod


# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: SLURM run scripts for running 
# Damaris dask example stats_3dmesh_dask on SLURM based clusters (e.g. CINICA G100)

# N.B. If 4 simulations are launched which each runs 4 iterations, 
#      and each adding a integer 1, 2, 3, or 4, distributed over the 4 simulations 
#      on each iteration, then: 
#      The mean of the first blocks will be 2.5 
#      and the variance for 4 iterations will be 1.333...

sleep 20 
# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=$HOME/mypylib:$PYTHONPATH

# MY_VAL=$1  this is set by the sbatch --export= command line

# We are assuming we are in the sub directory <INSTALL_PATH>/examples/damaris/python/SLURM 
# and the executables are one directory up, so move there:
cd ..


while [[ ! -f ./SLURM/stats_3dmesh_dask_slurm.xml ]] ; do
    echo "INFO: $PWD "
    echo "INFO: the Damaris input file does not seem to be available: ./SLURM/stats_3dmesh_dask_slurm.xml"
    sleep 5
done

cat ./SLURM/stats_3dmesh_dask_slurm.xml 

# To get rid of: "mpool.c:43   UCX  WARN  object 0x1d1dc40 was not returned to mpool ucp_requests"
export UCX_LOG_LEVEL=error

#  Usage: stats_3dmesh_dask <stats_3dmesh_dask.xml> [-i I] [-d D] [-r] [-s S]
#  -i  I    I is the number of iterations of simulation to run
#  -v  V    V is the value to add to each array element (default = 5)
#  -d  D    D is the number of domains to split data into (must divide into WIDTH perfectly from XML file)
#  -s  S    S is integer time to sleep in seconds between iterations
mpirun -np 48 -x PYTHONPATH=$PYTHONPATH  --oversubscribe --mca opal_warn_on_missing_libcuda 0  ./stats_3dmesh_dask ./SLURM/stats_3dmesh_dask_slurm.xml -i 4 -v $MY_VAL -d 4 -s 5

sleep 5
