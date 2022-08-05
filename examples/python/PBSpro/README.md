# Scripts for running multiple test case simulations

The test system used was PBSpro based running on IT4I Karolina 

I used:

```
module load Boost/1.77.0-GCC-10.2.0-Python-3.8.6
module load OpenMPI/4.1.1-GCC-10.2.0
```

```
export INSTALL_PREFIX=$HOME/local
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$INSTALL_PREFIX/lib64:$LD_LIBRARY_PATH
export PATH=$INSTALL_PREFIX/bin:$PATH
NUMCORES=8

XERCES_VERSION=3.2.2

wget --no-check-certificate https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-${XERCES_VERSION}.tar.gz
tar -xzf xerces-c-${XERCES_VERSION}.tar.gz
cd xerces-c-${XERCES_VERSION}
./configure --prefix=$INSTALL_PREFIX --disable-threads --disable-network
make -j${NUMCORES}
make install

cd ..
XSD_VERSION=4.0.0 
wget http://www.codesynthesis.com/download/xsd/4.0/xsd-${XSD_VERSION}+dep.tar.bz2
tar -xjf xsd-${XSD_VERSION}+dep.tar.bz2
cd xsd-${XSD_VERSION}+dep
make -j${NUMCORES} LDFLAGS="-L${INSTALL_PREFIX}/lib/" CFLAGS="-I ${INSTALL_PREFIX}/include" CXXFLAGS="-I ${INSTALL_PREFIX}/include/"
# N.B. If compilation fails with error to do with <<, add #include <iostream> to top of file libxsd-frontend/xsd-frontend/semantic-graph/elements.cxx
make install_prefix=$INSTALL_PREFIX install LDFLAGS="-L${INSTALL_PREFIX}/lib/" CFLAGS="-I ${INSTALL_PREFIX}/include" CXXFLAGS="-I ${INSTALL_PREFIX}/include/"
    
```


## Install mpi4py from source

```
pip3 install mpi4py --no-binary mpi4py --user --force --no-cache-dir
```
  

# Make sure we have set PYTHONPATH correctly - $HOME/mpyylib was set in Damaris CMake configuration
```
export PYTHONPATH=$HOME/mypylib:$(python -m site --user-site)
```
  
# Interactive testing examples 

## Example 1
  
```
salloc --no-shell -N 1 --ntasks-per-node=5 -A <project>  --partition=g100_usr_interactive
srun --ntasks=5  --jobid=5416506 ./3dmesh_py 3dmesh_py.xml -i 3 -v 2 -r


pip3 install mpi4py --no-binary mpi4py --user --force --no-cache-dir
mpirun -np 2 python -c "from   damaris4py.server import isserver ; print(isserver()) "

mpirun -np 2 python -c "import sys ; for path in sys.path: print(path)"
```

## Example 2
```
sacct -u jbowden0

salloc --no-shell -N 1 --ntasks-per-node=1 --cpus-per-task=1 --ntasks=7 -A <project>  --partition=g100_usr_interactive
srun --jobid=5412063 --ntasks-per-core=1 --cpus-per-task=1 --oversubscribe  --ntasks=1 dask-scheduler --scheduler-file "$HOME/dask_file.json"  &

salloc --no-shell -N 1 --ntasks-per-node=6 -A <project>  --partition=g100_usr_interactive
srun --jobid=5412063  --ntasks-per-core=1 --cpus-per-task=1 --oversubscribe  --ntasks=6 ./3dmesh_py_domains 3dmesh_dask.xml -i 4 -r -d 4
```

## Example 3
The driver script is:

```
stats_launcher.sh <num_sims>
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

