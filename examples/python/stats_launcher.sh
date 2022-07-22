#! /bin/bash

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: Master workflow OAR run script 
# Damaris dask example 3dmesh_dask_stats on Grid5000
#
# ./stats_launcher.sh [ NUMSIMS ] 
# NUMSIMS is optional and defaults to 4


# Get value for number of simulations to run from the command line (if present)
NUMSIMS=4
if [[ ! -z "$1" ]] ; then
  NUMSIMS=$1
fi

# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH

# This needs to match what is in the Damaris XML file
DASK_SCHEDULER_FILE=/home/jbowden/dask_file.json
DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'


if [[ -f $DASK_SCHEDULER_FILE ]] ; then
    python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"  &> /dev/null
    if [[ -f $DASK_SCHEDULER_FILE  ]] ; then
      rm $DASK_SCHEDULER_FILE
    fi
fi
# python3 -c "from dask.distributed import Client; client= Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s'); client.shutdown()"

echo "PWD = $PWD"
sleep 5

#####
# Launch the Dask scheduler - all jobs subsequent will attach to this scheduler
# The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> tag 
# of the simulations (launched in stats_launch_one_job.sh)
SCHED_JOB_ID=$(oarsub -p "cluster=paravance" -S "./stats_launch_scheduler.sh  $DASK_SCHEDULER_FILE" | sed -n 's/^.*=//p')
echo "Submitted Job $SCHED_JOB_ID  stats_launch_scheduler.sh"

sleep 30


#####
# Poll to wait until scheduler job has stared
STATERUNNING="not running"
echo "Waiting for Job $SCHED_JOB_ID to be in state = Running"
while [ "$STATERUNNING" == "" ]
do
  STATE=$(oarstat -f -j $SCHED_JOB_ID | grep "state =")
  STATERUNNING=$(echo $STATE  | grep "Running")
  STATEEND=$(echo $STATE  | grep "End")
  if [[ -z "$STATEEND" ]] ; then 
      echo $STATE
      echo "Scheduler Job $SCHED_JOB_ID  in state = End - exiting launcher"
      exit -1
      
  fi
  STATEERROR=$(echo $STATE  | grep "Error")
  if [[ -z "$STATEERROR" ]] ; then 
      echo $STATE
      echo "Scheduler Job $SCHED_JOB_ID  in state = Error - exiting launcher"
      exit -1
  fi
  echo $STATE
  sleep 30
done
echo "Scheduler Job $SCHED_JOB_ID  in state = Running"
sleep 60




#####
# Launch the simulations, passing in a different value to add to the dataset ($i)
JOBS_ARRAY=()
echo "We will be launching $NUMSIMS simulaitons"
for i in `seq 1 $NUMSIMS` ; do
    # sbatch slurm_launch_one_job.sh $i  # Use the correct submission command for your cluster's resource manager
    
    echo "Launching Job $i:"
    CURRENT_JOBNUM=$(oarsub -p "cluster=paravance" -S "./stats_launch_one_job.sh ${i}" | sed -n 's/^.*=//p')
    JOBS_ARRAY+=($CURRENT_JOBNUM)
    echo "    OAR JOBID: $CURRENT_JOBNUM"
done


#####
# Wait for jobs to finish
# This proceedure deletes elements from the bash array if the job id is found to be 
# in the End or Error state. 
# Requires specific output of the OAR scheduler command: oarstat -f -j <JID>
# state = End 
# or 
# state = Error

STARTSIZE=${#JOBS_ARRAY[@]}
echo "Waiting for $STARTSIZE jobs to finish"
while [ "$STARTSIZE" -gt 0 ]
do
    for JID in ${JOBS_ARRAY[@]};
    do
        if [[ ! -z "$JID" ]] ; then
            STATE=$(oarstat -f -j $JID | grep "state =")
            STATEEND=$(echo $STATE  | grep "End")
            if [[ ! -z "$STATEEND" ]] ; then 
              delete=($JID)
              JOBS_ARRAY=( "${JOBS_ARRAY[@]/$delete}" )
              STARTSIZE=$((STARTSIZE-1))
              echo "Jobs running =  $STARTSIZE"
            fi
            STATEERROR=$(echo $STATE  | grep "Error")
            if [[ ! -z "$STATEERROR" ]] ; then 
              delete=($JID)
              JOBS_ARRAY=( "${JOBS_ARRAY[@]/$delete}" )
              STARTSIZE=$((STARTSIZE-1))
              echo "Jobs running =  $STARTSIZE  (last one errored)"
            fi
            STATETERMINATED=$(echo $STATE  | grep "Terminated")
            if [[ ! -z "$STATETERMINATED" ]] ; then 
              delete=($JID)
              JOBS_ARRAY=( "${JOBS_ARRAY[@]/$delete}" )
              STARTSIZE=$((STARTSIZE-1))
              echo "Jobs running =  $STARTSIZE  (last one Terminated)"
            fi
        fi
    done
    sleep 10
    echo "Waiting for $STARTSIZE jobs to finish"
done



#####
# Collect the summary statistics - save to file or print to screen (if small enough)
echo "Collect the summary statistics results"
FINALIZE_JOB_ID=$(oarsub -t day -p "cluster=paravance" -S "./stats_get_results.sh $DASK_SCHEDULER_FILE" | sed -n 's/^.*=//p')
STATERUNNING="Running"
while [[ ! -z "$STATERUNNING"  || ! -z "$STATEWAITING"  ||  ! -z "$STATETOLAUNCH" ]]
do
  STATE=$(oarstat -f -j $FINALIZE_JOB_ID | grep "state =")
  STATERUNNING=$(echo $STATE  | grep "Running")
  STATEWAITING=$(echo $STATE  | grep "Waiting")
  STATETOLAUNCH=$(echo $STATE  | grep "toLaunch")
  STATEEND=$(echo $STATE  | grep "End")
  if [[ ! -z "$STATEEND" ]] ; then 
      echo $STATE
      break
  fi
  STATEERROR=$(echo $STATE  | grep "Error")
  if [[ ! -z "$STATEERROR" ]] ; then 
      echo $STATE
      break
  fi
  echo $STATE
  sleep 10
done


#####
# Now remove the Dask job scheduler job
echo "Removing the Dask job scheduler job"
# oardel  $SCHED_JOB_ID

# DASK_SCHEDULER_FILE=/home/jbowden/dask_file.json
# DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'
python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"

