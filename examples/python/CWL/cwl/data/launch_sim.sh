#!/bin/bash
echo "DEBUG SIM"

echo "Simulation started with arguments" $1 $2 $3 $4 $5
echo $4
echo `pwd`

export DASK_SCHED=$4

sed -i 's|scheduler-file="scheduler.json"|scheduler-file="'"$DASK_SCHED"'"|' $2 
sed -i 's|file="stats_3dmesh_dask.py"|file="'"$5"'"|' $2

mpirun -np 4 $1 $2 -i 3 -v $3 -d 4 -s 2 > sim_out.txt 2> sim_err.txt #> /dev/null 2>&1
# #mpirun -np 4 ./stats_3dmesh_dask stats_3dmesh_dask.xml -i 4 -v $1 -d 4 -s 5

echo "Simulation $3 ended" > sim_ended.txt