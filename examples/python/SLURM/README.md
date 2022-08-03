# Scripts for running multiple test case simulations

The test system used was the SLURM based running on Galileo 100 


```


## Installing openblas (w. INTERFACE64=1 BINARY=64 CORE=SKYLAKEX), numpy (requires cython) and boost numpy, Dask
git clone https://github.com/xianyi/OpenBLAS.git
cd OpenBLAS
edit: Makefile.rule : INTERFACE64=1 BINARY=64
make CORE=SKYLAKEX PREFIX=$INSTALL_PREFIX install

## Install NumPy
export NPY_USE_BLAS_ILP64=1
export NPY_BLAS_ILP64_ORDER=openblas_ilp64
export NPY_LAPACK_ILP64_ORDER=openblas_ilp64

python -m pip install --user --upgrade cython
git clone https://github.com/numpy/numpy.git
cd numpygit submodule update --init
edit: site.cfg -> choose openblas 64 bit verion but without 64 bit extension
python setup.py build --fcompiler=gfortran
python setup.py install --prefix=$HOME/.local

## Install boost with python and numpy support
RUN mkdir buildboost \
    && cd buildboost \
    && apt-get install -y python3-mpi4py python3-numpy \
    && wget https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz \
    && tar -xf boost_1_79_0.tar.gz \
    && cd boost_1_79_0 \
    && ./bootstrap.sh --with-python=$(which python) --prefix=${INSTALL_PREFIX} \
    && ./b2 install \
    && cd ../.. \
    && rm -fr ./buildboost
```

## Install mpi4py from source

```
pip3 install mpi4py --no-binary mpi4py --user --force --no-cache-dir
```
  

# Make sure we have set PYTHONPATH correctly - $HOME/mpyylib was set in Damaris CMake configuration
```
export PYTHONPATH=$HOME/mpyylib:$(python -m site --user-site)
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

