#!/bin/sh
export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH 
export PATH=$HOME/local/bin:$PATH

#Run the example on two servers (dedicated cores) and two cliets
mpirun -np 32 ./2dmesh "2dmesh.xml"


