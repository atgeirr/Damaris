#!/bin/sh
export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH 
export PATH=$HOME/local/bin:$PATH

#Run the example:
#mpirun -np 6 ./image "image.xml"

#mpirun -np 4 ./sector "sector.xml"

mpirun -np 8 ./plate "plate.xml"

#mpirun -np 4 ./lollipop "lollipop.xml"
