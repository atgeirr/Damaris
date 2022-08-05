#! /bin/bash

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: Master workflow PBS run script 
# Damaris dask example 3dmesh_dask_stats on IT4Inovation Karolina cluster
#
# ./stats_launcher.sh [ NUMSIMS ] 
# NUMSIMS is optional and defaults to 4

INSTALL_PREFIX=$HOME/local
PROJECT_ID=DD-22-20
PROJECT_QUEUE=qcpu_eurohpc
# N.B. There is a file named stat_modules.sh that loads modules and other paths for all submitted jobs
# This needs to match what is in the Damaris XML file - we use a sed command to replace it bon line 22 below
DASK_SCHEDULER_FILE=$HOME/dask_file.json
DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'
# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=$HOME/mypylib:$(python -m site --user-site)
# Make sure our Damaris XML file points to the correct Dask scheduler file
#  N.B. We assume we are in the PBS directory (one directory up from the executables
sed -i "s|scheduler-file=\".*\" |scheduler-file=\"${DASK_SCHEDULER_FILE}\" |g" stats_3dmesh_dask_PBS.xml



# Get value for number of simulations to run from the command line (if present)
NUMSIMS=4
if [[ ! -z "$1" ]] ; then
  NUMSIMS=$1
fi


python3 -c "from  damaris4py.server import getservercomm"
if [[ "$?" != "0" ]] ; then
  echo "$0 : Will exit as we cannot find the Python module damaris4py"
  exit 0
fi



#####
echo "Shutdown and remove any previous Dask scheduler "
if [[ -f $DASK_SCHEDULER_FILE ]] ; then
    echo "The Dask scheduler seems to be running: $DASK_SCHEDULER_FILE"
    echo "Please wait for it to finish, or remove it, or set the DASK_SCHEDULER_FILE value to another file"
    echo "To end the scheduler, use:"
    echo "python3 -c \"from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()\""
fi

echo "PWD = $PWD"
sleep 5

#####
# Launch the Dask scheduler - all jobs subsequent will attach to this scheduler
# The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> tag 
# of the simulations (launched in stats_launch_one_job.sh)
# -V means export the full environment use -v {list} to export specific variables i.e. DASK_SCHEDULER_FILE
SCHED_JOB_ID=$(qsub -v DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE -A $PROJECT_ID -q $PROJECT_QUEUE stats_launch_scheduler.sh)
sleep 2
echo "Scheduler Job is: $SCHED_JOB_ID "
if [[ -z "$SCHED_JOB_ID" ]] ; then 
      echo "Scheduler Job could not be launched - check script stats_launch_scheduler.sh exists"
      exit -1
fi
echo "Submitted Job $SCHED_JOB_ID  stats_launch_scheduler.sh"
sleep 30



#####
# Poll to wait until scheduler job has stared
STATERUNNING=""
while [ "$STATERUNNING" == "" ]
do
  echo "Waiting for Job $SCHED_JOB_ID to be in state = Running"
  STATESCH=$(qstat -x $SCHED_JOB_ID | grep  $SCHED_JOB_ID )
  
  STATESCH_FIN=$(echo $STATESCH | grep " F " )
  # If empty the job has finished
  if [[ ! -z "$STATESCH_FIN" ]] ; then 
      echo $STATESCH
      echo "Scheduler Job $SCHED_JOB_ID  in state = F finished - exiting $0"
      exit -1
  fi
  STATERUNNING=$(echo $STATESCH  | grep " R ")
  sleep 10
done
echo "INFO: $STATERUNNING"
echo "INFO: Scheduler Job $SCHED_JOB_ID in state = Running"
echo "INFO: Checking if scheduler file exists: $DASK_SCHEDULER_FILE"
sleep 30

if [[ ! -f $DASK_SCHEDULER_FILE  ]] ; then
    echo "ERROR: Scheduler file does not exist: $DASK_SCHEDULER_FILE"
    echo "Sleeping for a bit !"
    sleep 20
fi
if [[ ! -f $DASK_SCHEDULER_FILE  ]] ; then
    echo "ERROR: Scheduler file does not exist: $DASK_SCHEDULER_FILE"
    echo "Exiting workflow!"
    qdel $SCHED_JOB_ID
    exit -1
fi




#####
# Launch the simulations, passing in a different value to add to the dataset using
# the job array ${PBS_ARRAY_INDEX} within the script stats_launch_one_job.sh
# N.B. these jobs implicitly use a Dask scheduler, which is specified in the Damaris
# XML file <pyscript> tag - it must match the path to DASK_SCHEDULER_FILE

echo "We will be launching $NUMSIMS simulations as a PBS Job Array"
# Could use -W depend=after:$SCHED_JOB_ID
# N.B. $NUMSIMS should be less than or equal to 1500 
ARRAY_JID=$(qsub -J 1-$NUMSIMS -v PYTHONPATH=$PYTHONPATH -A $PROJECT_ID -q $PROJECT_QUEUE stats_launch_one_job.sh   | sed -n 's/^.*job //p')



#####
# Wait for jobs to finish
# This proceedure deletes elements from the bash array if the job id is found to be 
# in the End or Error state. 
# Requires specific output of the OAR scheduler command: oarstat -f -j <JID>
# state = End 
# or 
# state = Error

echo "Waiting for $NUMSIMS jobs in job array to finish"
ARRAY_FIN=""
while [[ -z "$ARRAY_FIN" ]]
do
    STATE=$(qstat -x $ARRAY_JID | grep  -F $ARRAY_JID )
    ARRAY_FIN=$(echo $STATE | grep " F " ) 
    
    # Check that the scheduler job is running            
    STATESCH=$(qstat -x $SCHED_JOB_ID | grep  $SCHED_JOB_ID )
    STATESCH_FIN=$(echo $STATESCH | grep " F " )
    # If empty the scheduler job has finished
    if [[ ! -z "$STATESCH_FIN" ]] ; then 
      echo $STATESCH
      echo "Scheduler Job $SCHED_JOB_ID  in state = F finished - exiting $0"
      exit -1
    fi
    # Check on load on scheduler node
    check-pbs-jobs --jobid $SCHED_JOB_ID  --print-load --print-processes
    sleep 10
    echo "Waiting for $NUMSIMS jobs in job array to finis"
done


# We could use -W depend=afterok:$ARRAY_JID
#####
# Collect the summary statistics - save to file or print to screen (if small enough)
echo "Collect the summary statistics results"
RESULTS_JOB_ID=$(qsub -v DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE, PYTHONPATH=$PYTHONPATH  -A $PROJECT_ID -q $PROJECT_QUEUE  stats_get_results.sh )
while true
do
   STATE=$(squeue -x $RESULTS_JOB_ID | grep $RESULTS_JOB_ID)
   RESULTS_JOB_FIN=$(echo $STATE | grep " F " )  
    # If not empty then F was found
    if [[ ! -z "$RESULTS_JOB_FIN" ]] ; then 
      echo $RESULTS_JOB_FIN
      echo "Data collection job $RESULTS_JOB_ID has ended!"
      break
    fi

  echo "Waiting for results statistics PBS JOBID: $RESULTS_JOB_ID"
  sleep 10
done


#####
# Now remove the Dask job scheduler job
echo "Removing the Dask job scheduler job"
python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"

