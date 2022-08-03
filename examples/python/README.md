# Damaris with Python and Dask Support

## Running the Examples
There are 3 examples in the examples/python directory that can be useful to understand what is happening. The examples are available in the Damaris install directory under /examples/damaris/python. 
  
Please note: The examples are only installed if CMake was configured with -DENABLE_EXAMPLES=ON.
             And, if they are installed in a read-only system directory, you will need to copy them
             to your local directory area. They do not write a lot of data.

### Example 1  
A very basic example that only uses Python functionality (no Dask).
From directory     : <INSTALL_DIR>/examples/damaris/python
Executable         : 3dmesh_py 
Damaris XML config : 3dmesh_py.xml
Python script      : 3dmesh_py.py

```
Usage: 3dmesh_py <3dmesh_py.xml> [-v] [-r] [-s X]
-v  <X>    X = 0 default, do not print arrays
           X = 1 Verbose mode, prints arrays
           X = 2 Verbose mode, prints summation of arrays
-r         Array values set as rank of process\n");
-s  <Y>    Y is integer time to sleep in seconds between iterations
-i  <I>    I is the number of iterations of simulation to run\

 mpirun --oversubscribe -np 5 ./3dmesh_py 3dmesh_py.xml -i 3 -v 2 -r
```
  
Expected result:  
For the same iteration, the sum output from C++ printed to screen should match the value computed by the 3dmesh_py.py script

N.B. This example will fail if run over multiple nodes, or with multiple Damaris server cores, as the Python script only has access to NumPy data of the clients that are working with the server core.

To see this in action, change <dedicated cores="1" nodes="0" /> to <dedicated cores="2" nodes="0" /> in file 3dmesh_py.xml and run:
```
 # Note the extra rank requested: -np 6, to account for the extra Damaris server core.
 mpirun --oversubscribe -np 6 ./3dmesh_py 3dmesh_py.xml -i 3 -v 2 -r
```
  
You will notice 2 outputs from the Python script on each iteration, one output for each Damaris server, showing a sum, and still only a single output of the sum from C++. You will notice that the sum of the 2 Python outputs will be equal to the C++ total.
```
C++    iteration  0 , Sum () = 98304
...
Python iteration  0 , Sum() =  16384
Python iteration  0 , Sum() =  81920

```
This shows thae distributed nature of the data, with each Damaris server looking after data from particular Damaris server ranks (2 clients per server in this case).

### Example 2
From directory     : <INSTALL_DIR>/examples/damaris/python
Executable         : 3dmesh_py_domains 
Damaris XML config : 3dmesh_dask.xml
Python script      : 3dmesh_dask.py
  
An example of using Python integration with Dask distributed. This version sets deals with the distributed data on the Python side by creating a dask.array, so it can sum of the data in the blocks over multiple Damaris server cores and even over distributed nodes.
    
To run this example, a Dask scheduler needs to be spun up:
```
   # N.B. best to run this in a separate xterm session as it is quite verbose
   dask-scheduler --scheduler-file "$HOME/dask_file.json" &
```
  
The --scheduler-file argument must match what is specified in the Damaris XML file <pyscript scheduler-file="" > tag.
  
To run the simulation:   
Assumes 4 Damaris clients and 2 Damaris server cores as per the xml file   
i.e. the XML input file (3dmesh_dask.xml) ] contains ```<dedicated cores="2" nodes="0" />``` in the <architecture> tag.
  
```
Usage: 3dmesh_py_domains  <3dmesh_dask.xml> [-i I] [-d D] [-r] [-s S];
-i  I    I is the number of iterations of simulation to run
-r         Array values set as rank of process
-d  D    D is the number of domains to split data into (must divide into WIDTH value in XML file perfectly)
-s  S    S is integer time to sleep in seconds between iterations

mpirun --oversubscribe -np 6 ./3dmesh_py_domains 3dmesh_dask.xml -i 10 -r -d 4
``` 
N.B. Set the global mesh size values WIDTH, HEIGHT and DEPTH using the XML input file.
     Set the name of the Python script to run in the <pyscript ... file="3dmesh_dask.py" ...> tag
  
The simulation code (via Damaris pyscript class) will create the Dask workers (one per Damaris server core)  and have them connect to the Dask scheduler. The simulation code will remove the workers at the end of the execution, unless  keep-workers="yes" is specified in 3dmesh_dask.xml <pyscript> tag.
  
To stop the scheduler:
```
DASK_SCHEDULER_FILE=$HOME/dask_file.json  # must match what you are using above
DASK_SCHED_STR=\'$DASK_SCHEDULER_FILE\'
python3 -c "from dask.distributed import Client; client= Client(scheduler_file=$DASK_SCHED_STR, timeout='2s'); client.shutdown()"
```
  
### Example 3
This example requires an OAR cluster to launch jobs on. There should be equivalent scripts for SLURM and PBS schedulers.
From directory     : <INSTALL_DIR>/examples/damaris/python/OAR
Workflow launcher  : stats_launcher.sh
Scheduler scripts  : stats_launch_scheduler.sh stats_launch_one_job.sh stats_get_results.sh
Executable         : stats_3dmesh_dask
Damaris XML config : stats_3dmesh_dask.xml
Python script      : stats_3dmesh_dask.py & stats_get_results.py
  
The example runs multiple versions of the same simulation, with different input values and computes running statistics (averages and variances) of the exposed Damaris field data (cude_d), a rank 3 dimension double precision array. Each simulation runs multiple iterations, each iteration presenting the same value (unique for the simulation instance) to the running statistic arrays. 
  
The stats_launcher.sh workflow launcher script goes through the following steps:
1. Get value for number of simulations to run from the command line (if present).
2. Shutdown and remove any previous Dask scheduler.
3. Launch the Dask scheduler  (stats_launch_scheduler.sh) - all jobs subsequent will attach to this 
    scheduler. The name of the scheduler-file must match what is stored in the Damaris XML <pyscript> 
    tag  of the simulations (launched in stats_launch_scheduler.sh).
4. Poll to wait until Dask scheduler job has stared.
5. Launch the simulations (stats_launch_one_job.sh), passing in a different value to add to the dataset ($i)
   N.B. these jobs implicitly use a Dask scheduler, which is specified in the Damaris
   XML file <pyscript> tag - it must match the path to DASK_SCHEDULER_FILE
6. Wait for OAR jobs to finish.
7. Collect the summary statistics (stats_get_results.sh) - stats_get_results.py will print to screen (data is small enough).
8. Remove the Dask job scheduler job.
  
To run the example, log in to Grid5000 and cd to <INSTALL_DIR>/examples/damaris/python/OAR
```
cd <INSTALL_DIR>/examples/damaris/python/OAR
./stats_launcher.sh 4
```
  
The results are printed to the job output file stats_finalize_%jobid%.std file and if 4 simulations are launched, each one running 4 iterations and on each iteration adding a value 1,2,3,4, then the mean should be 2.5 and the variance 1.333...
