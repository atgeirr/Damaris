#!/bin/sh

export PATH=/home/hadi/local/bin:$PATH
export LD_LIBRARY_PATH=/home/hadi/visit2.10.3/2.10.3/linux-x86_64/lib/osmesa:/home/hadi/visit2.10.3/2.10.3/linux-x86_64/lib:./visit2.10.3/bin/../current/linux-x86_64/lib:/home/hadi/local/lib:$LD_LIBRARY_PATH
export VISIT_MESA_LIB=/home/hadi/visit2.10.3/2.10.3/linux-x86_64/lib/osmesa/libGL.so.1



#rm Snapshot*
#rm ~/.visit/simulations/*.sim2

mpirun -np 2 ./cube
