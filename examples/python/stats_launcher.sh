#! /bin/bash


#####
# Launch the Dask scheduler - all jobs subsequent will attach to this scheduler
# The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> tag 
# of the simulations (launched in stats_launch_one_job.sh)
SCHED_JOB_ID=$(oarsub -t day -p "cluster=paravance" -S stats_launch_scheduler.sh | sed -n 's/^.*=//p')
sleep 5


#####
# Poll to wait until scheduler job has stared
STATERUNNING="not running"
while [ "$STATERUNNING" == "" ]
do
  STATE=$(oarstat -f -j $SCHED_JOB_ID | grep "state =")
  STATERUNNING=$(echo $STATE  | grep "Running")
  STATEEND=$(echo $STATE  | grep "End")
  if [[ -z "$STATEEND" ]] ; then 
      echo $STATE
      break
  fi
  STATEERROR=$(echo $STATE  | grep "Error")
  if [[ -z "$STATEERROR" ]] ; then 
      echo $STATE
      break
  fi
  echo $STATE
  sleep 1
done

sleep 5


#####
# Launch the simulations, passing in a different value to add to the dataset ($i)
JOBS_ARRAY=()
for i in `seq 1 4` do
    # sbatch slurm_launch_one_job.sh $i  # Use the correct submission command for your cluster's resource manager
    CURRENT_JOBNUM=$(oarsub -t day -p "cluster=paravance"  -S stats_launch_one_job.sh $i | sed -n 's/^.*=//p')
    JOBS_ARRAY+=($CURRENT_JOBNUM)
done


#####
# Wait for jobs to finish
# This proceedure deletes elements from the bash array if the job id is found to be 
# in the End or Error state. Completly relies of the OAR scheduler oarstat -f -j <JID> output:
# state = End 
# or 
# state = Error
STARTSIZE=${JOBS_ARRAY[@]}
while [ "$STARTSIZE" -gt 0 ]
do
  for JID in ${JOBS_ARRAY[@]};
  do
    if [[ ! -z "$JID" ]] ; then
        STATE=$(oarstat -f -j $JID | grep "state =")
        STATEEND=$(echo $STATE  | grep "End")
        if [[ -z "$STATEEND" ]] ; then 
          delete=($JID)
          $JOBS_ARRAY[@]/$delete
          STARTSIZE=$((STARTSIZE-1))
        fi
        STATEERROR=$(echo $STATE  | grep "Error")
        if [[ -z "$STATEERROR" ]] ; then 
          delete=($JID)
          $JOBS_ARRAY[@]/$delete
          STARTSIZE=$((STARTSIZE-1))
        fi
        done
    fi
    
  sleep 10
done



#####
# Collect the summary statistics - save to file or something (print to screen)
FINALIZE_JOB_ID=$(oarsub -t day -p "cluster=paravance" -S stats_finalize.sh | sed -n 's/^.*=//p')
STATERUNNING="Running"
while [ "$STATERUNNING" == "Running" ]
do
  STATE=$(oarstat -f -j $FINALIZE_JOB_ID | grep "state =")
  STATERUNNING=$(echo $STATE  | grep "Running")
  STATEEND=$(echo $STATE  | grep "End")
  if [[ -z "$STATEEND" ]] ; then 
      echo $STATE
      break
  fi
  STATEERROR=$(echo $STATE  | grep "Error")
  if [[ -z "$STATEERROR" ]] ; then 
      echo $STATE
      break
  fi
  echo $STATE
  sleep 1
done


#####
# Now remove the Dask job scheduler job
oardel  $SCHED_JOB_ID