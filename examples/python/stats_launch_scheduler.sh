#! /bin/bash
#OAR -l /nodes=1/core=16,walltime=01:00
#OAR -O scheduler_%jobid%.std
#OAR -E scheduler_%jobid%.err
#OAR -n scheduler


dask-scheduler --scheduler-file /home/jbowden/dask_files.json  

sleep 10



