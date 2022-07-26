# Scripts for running multiple test case simulations

The driver script is:

```
stats_launcher.sh
```

The driver script goes through the following steps:

1. Get value for number of simulations to run from the command line (if present).

2. Shutdown and remove any previous Dask scheduler.

3. Launch the Dask scheduler - all jobs subsequent will attach to this scheduler
   The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> tag 
   of the simulations (launched in stats_launch_one_job.sh).

4. Poll to wait until Dask scheduler job has stared.

5. Launch the simulations, passing in a different value to add to the dataset ($i)
   N.B. these jobs implicitly use a Dask scheduler, which is specified in the Damaris
   XML file <pyscript> tag - it must match the path to DASK_SCHEDULER_FILE

6. Wait for OAR jobs to finish.

7. Collect the summary statistics - save to file or print to screen (if small enough).

8. Remove the Dask job scheduler job.

