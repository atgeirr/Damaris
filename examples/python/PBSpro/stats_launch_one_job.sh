#!/bin/bash
#PBS -l walltime=00:05:00
#PBS -l select=1
#PBS -j oe
#PBS -m n
#PBS -N 3dmesh_stats

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: PBSpro run scripts for running 
# Damaris dask example stats_3dmesh_dask on PBSpro based clusters (e.g. IT4I Karolina)

# N.B. If 4 simulations are launched which each runs 4 iterations, 
#      and each adding a integer 1, 2, 3, or 4, distributed over the 4 simulations 
#      on each iteration, then: 
#      The mean of the first blocks will be 2.5 
#      and the variance for 4 iterations will be 1.333...

cd $PBS_O_WORKDIR
# JID_NUM_ONLY=$(echo  $PBS_JOBID | sed 's|\[.*||g')
# mkdir -p ${JID_NUM_ONLY}/${PBS_ARRAY_INDEX}
# cd ${JID_NUM_ONLY}/${PBS_ARRAY_INDEX}
. stats_modules.sh
 

# To be passed in on qsub command line using -v
echo "PYTHONPATH=$PYTHONPATH "
echo "DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE"

sleep 20 
# Set path to Damaris Python module (for getting server MPI communicator)
# export PYTHONPATH=$HOME/mypylib:$(python -m site --user-site)

# PBS_ARRAY_INDEX is set by the job array of PBS
MY_VAL=$( expr ${PBS_ARRAY_INDEX}  % 4 + 1)


# We are assuming we are currently in the sub directory <INSTALL_PATH>/examples/damaris/python/PBSpro 
# and the executables are one directory up, so move there:
cd ..


while [[ ! -f ./PBSpro/stats_3dmesh_dask_PBS.xml ]] ; do
    echo "INFO: $PWD "
    echo "INFO: the Damaris input file does not seem to be available: ./PBSpro/stats_3dmesh_dask_PBS.xml"
    sleep 5
done

cat ./PBSpro/stats_3dmesh_dask_PBS.xml 

# To get rid of: "mpool.c:43   UCX  WARN  object 0x1d1dc40 was not returned to mpool ucp_requests"
export UCX_LOG_LEVEL=error
# --mca opal_warn_on_missing_libcuda 0 
# --mca mpi_warn_on_fork 0  # Suppress the warning that is probably due to std::system() call to launch the dask-workers

#  Usage: stats_3dmesh_dask <stats_3dmesh_dask.xml> [-i I] [-d D] [-r] [-s S]
#  -i  I    I is the number of iterations of simulation to run
#  -v  V    V is the value to add to each array element (default = 5)
#  -d  D    D is the number of domains to split data into (must divide into WIDTH perfectly from XML file)
#  -s  S    S is integer time to sleep in seconds between iterations
mpirun -np $NCPUS -x PYTHONPATH=$PYTHONPATH  --mca mpi_warn_on_fork 0 --oversubscribe  ./stats_3dmesh_dask ./PBSpro/stats_3dmesh_dask_PBS.xml -i 4 -v $MY_VAL -d 4 -s 5

sleep 5
