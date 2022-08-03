#! /bin/bash

# Author: Josh Bowden, INRIA
# Date: 20/07/2022
# Description: Master workflow OAR run script 
# Damaris dask example 3dmesh_dask_stats on Grid5000
#
# ./stats_launcher.sh [ NUMSIMS ] 
# NUMSIMS is optional and defaults to 4

INSTALL_PREFIX=/g100/home/userexternal/jbowden0/local

# Get value for number of simulations to run from the command line (if present)
NUMSIMS=4
if [[ ! -z "$1" ]] ; then
  NUMSIMS=$1
fi


# Set path to Damaris Python module (for getting server MPI communicator)
export PYTHONPATH=$HOME/mypylib:$(python -m site --user-site)


python3 -c "from  damaris4py.server import getservercomm"
  if [[ "$?" != "0" ]] ; then
      echo "$0 : Will exit as we cannot find the Python module damaris4py"
      exit 0
  fi
  
  

# This needs to match what is in the Damaris XML file
DASK_SCHEDULER_FILE=$HOME/dask_file.json
DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'

#####
echo "Shutdown and remove any previous Dask scheduler "
if [[ -f $DASK_SCHEDULER_FILE ]] ; then
    python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"  &> /dev/null
    if [[ -f $DASK_SCHEDULER_FILE  ]] ; then
      rm $DASK_SCHEDULER_FILE
    fi
fi

echo "PWD = $PWD"
sleep 5

#####
# Launch the Dask scheduler - all jobs subsequent will attach to this scheduler
# The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> tag 
# of the simulations (launched in stats_launch_one_job.sh)
SCHED_JOB_ID=$(sbatch --export=ALL,DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE  ./stats_launch_scheduler.sh  | sed -n 's/^.*job //p')
sleep 2
echo "Scheduler Job is: $SCHED_JOB_ID "
if [[ -z "$SCHED_JOB_ID" ]] ; then 
      echo "Scheduler Job $SCHED_JOB_ID  in state = End - exiting launcher"
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
  STATESCH=$(squeue --user=$USER | grep $SCHED_JOB_ID  )
  # If empty the job has finished
  if [[ -z "$STATESCH" ]] ; then 
      echo $STATESCH
      echo "Scheduler Job $SCHED_JOB_ID  in state = End - exiting launcher"
      exit -1
  fi
  STATERUNNING=$(echo $STATESCH  | grep "$USER R")
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
    scancel $SCHED_JOB_ID
    exit -1
fi


sed -i "s|scheduler-file=\".*\" |scheduler-file=\"${DASK_SCHEDULER_FILE}\" |g" stats_3dmesh_dask_slurm.xml


#####
# Launch the simulations, passing in a different value to add to the dataset ($i)
# Add the JOB ID to the JOBS_ARRAY so we can poll them to see if they have completed
# N.B. these jobs implicitly use a Dask scheduler, which is specified in the Damaris
# XML file <pyscript> tag - it must match the path to DASK_SCHEDULER_FILE
JOBS_ARRAY=()
echo "We will be launching $NUMSIMS simulations"
for i in `seq 1 $NUMSIMS` ; do
    # sbatch slurm_launch_one_job.sh $i  # Use the correct submission command for your cluster's resource manager
    
    echo "Launching Job $i:"
    CURRENT_JOBNUM=$(sbatch  --export=ALL,MY_VAL=${i} stats_launch_one_job.sh  | sed -n 's/^.*job //p')
    JOBS_ARRAY+=($CURRENT_JOBNUM)
    echo "    SLURM JOBID: $CURRENT_JOBNUM"
    sleep 5
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
            STATE=$(squeue --user=$USER | grep $JID)
            if [[ -z "$STATE" ]] ; then 
              delete=($JID)
              JOBS_ARRAY=( "${JOBS_ARRAY[@]/$delete}" )
              STARTSIZE=$((STARTSIZE-1))
              echo "Jobs running =  $STARTSIZE"
            fi
            # Check that the scheduler job is running            
            STATESCH=$(squeue --user=$USER | grep $SCHED_JOB_ID  )
            if [[ -z "$STATESCH" ]] ; then 
                echo $STATESCH
                echo "Scheduler Job $SCHED_JOB_ID  in state = End - exiting launcher"
                for JIDOUT in ${JOBS_ARRAY[@]};
                do
                    if [[ ! -z "$JIDOUT" ]] ; then
                        scancel $JIDOUT
                    fi
                done
                exit -1
            fi
        fi
    done
    sleep 10
    echo "Waiting for $STARTSIZE jobs to finish"
done



#####
# Collect the summary statistics - save to file or print to screen (if small enough)
echo "Collect the summary statistics results"
FINALIZE_JOB_ID=$(sbatch --export=ALL,DASK_SCHEDULER_FILE=$DASK_SCHEDULER_FILE stats_get_results.sh  | sed -n 's/^.*job //p')
STATERUNNING="Running"
while [[ ! -z "$STATERUNNING"  || ! -z "$STATEWAITING"  ]]
do
  STATE=$(squeue --user=$USER | grep $FINALIZE_JOB_ID)
  # If empty the job has finished
  if [[ -z "$STATE" ]] ; then 
      echo $STATE
      echo "Data collection job $FINALIZE_JOB_ID has ended!"
      break
  fi
  STATERUNNING=$(echo $STATE  | grep "$USER R")
  STATEWAITING=$(echo $STATE  | grep "$USER PD")
  
  echo "Waiting for summary statistics SLURM JOBID: $FINALIZE_JOB_ID"
  sleep 10
done


#####
# Now remove the Dask job scheduler job
echo "Removing the Dask job scheduler job"
# oardel  $SCHED_JOB_ID

# DASK_SCHEDULER_FILE=/home/jbowden/dask_file.json
# DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'
python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"

