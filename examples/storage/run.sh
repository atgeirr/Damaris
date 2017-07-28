#!/bin/sh
export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH 
export PATH=$HOME/local/bin:$PATH

#Run the example:
mpirun -np 12 ./2dmesh "2dmesh.xml"
#mpirun -np 32 ./3dmesh "3dmesh.xml"
#mpirun -np 6 ./vector "vector.xml"
#mpirun -np 3 ./mandelbulb-hdf5 "mandelbulb-hdf5.xml"
#mpirun -np 3 ./life-hdf5 "life-hdf5.xml"
#mpirun -np 2 ./blocks-hdf5 "blocks-hdf5.xml"

