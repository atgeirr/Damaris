# Python code: 3dmesh_dask.py
# Author: Josh Bowden, Inria
# Description: Part of the Damaris examples of using Python integration with Dask distributed
# To run this example, a Dask scheduler needs to be spun up:
# 
#   dask-scheduler --scheduler-file "/home/user/dask_file.json" &
#
# The --scheduler-file argument must match what is int the Damaris XML file 
# <pyscript> tag
# 
# Then run the simulation: (assumes 4 Damaris clients and 2 Damaris server cores as per the xml file)
#
#   mpirun --oversubscribe --host ubu20-hvvm-c -np 6 ./3dmesh_py 3dmesh_dask.xml -i 3 -v 2 -r
#
# This will create the Dask workers (one per Damaris server core) and have them connect to 
# the Dask scheduler. The simulation code will remove the workers at the end of the execution, 
# unless  keep-workers="yes" is specified in 3dmesh_dask.xml <pyscript> tag
# 

import numpy as np
np.set_printoptions(threshold=np.inf)


# N.B. This file is read by each Damaris server process on each iteration that is 
#      specified by the frequency="" in the <pyscript> XML sttribute.
# 
# DD (AKA Damaris Data) is a dictionary that has been filled by the 
# Damaris server process with NumPy arrays that point to the Damaris data variables 
# that are exposed in the simulation. The Damaris class that implements 
# this is PyAction, found in the src/scripts/ and include/damaris/scripts 
# directories. Damaris <variables> must be exposed to the Python <pyscript>
# XML element by including its name i.e. "MyPyAction"" in the following example:
# 
#    <variable name="cube_i" type="scalar" layout="cells_whd_wf" mesh="mesh" 
#                        centering="nodal" script="MyPyAction" />
#
#    <scripts>
#       <pyscript name="MyPyAction" file="3dmesh_py.py" language="python" frequency="1"
#            scheduler-file="/home/user/dask_file.json" nthreads="1" keep-workers="no" 
#            timeout="4" />
#    </scripts>
#    N.B. Setting nthreads="openmp" will set the dask worker threads
#         to the value obtained from opm_get_num_threads()
# 
#
# The Damaris server processes also present three dictionaries containing metadata 
# so that we can access the data
# DD['damaris_env'].keys()     - The global Damaris environment data
#       (['is_dedicated_node',    # 
#         'is_dedicated_core',    # 
#         'servers_per_node',     # Number of Damaris server ranks per node
#         'clients_per_node',     # Number of Damaris client ranks per node
#         'ranks_per_node',       # Total number of ranks per node
#         'cores_per_node',       # Total number of ranks per node (yes, the same as above)
#         'number_of_nodes',      # The total number of nodes used in the simulation
#         'simulation_name',      # The name of the simulation (specified in Damaris XML file)
#         'simulation_magic_number' # Unique number for a simulation run (used in constructing name of Dask workers.)
#       ])
#
# DD['dask_env'].keys()  - The Dask environment data, 
#       (['dask_scheduler_file',    # if an empty string then no Dask scheduler was found
#         'dask_workers_name',      # Each simulation has a uniquely named set of workers
#         'dask_nworkers',          # The total number of dask workers (== 'servers_per_node' x 'number_of_nodes')
#         'dask_threads_per_worker' # Dask workers can have their own threads. Specify as nthreads="1" in Damris XML file
#       ]) 
#
# DD['iteration_data'].keys() - A single simulation iteration. 
#                        Contains the iteration number and a lsit of sub-dictionaries, 
#                        one for each *Damaris variable* that has been exposed to the Python 
#                        interface. i.e. specified with the script="MyAction" in example the example above
#       (['iteration',              # The iteration number as an integer.
#          'cube_i',                # A Damaris variable dictionary
#          'cube_f',                # A Damaris variable dictionary
#          #'last_iter'             # This is not present in dictionary
#       ])
#
# A Damaris variable dictionary has the following structure
# DD['iteration_data']['cube_i'].keys()
#       (['numpy_data', 
#         'block_source', 
#         'block_domains', 
#         'type_string'
#        ])
#
# And, finally, the NumPy data is present in blocks, given by keys constructed as described below
# DD['iteration_data']['cube_i']['numpy_data'].keys()
#        (['P0_B0',  
#          'P1_B0'
#        ])
#  Damaris NumPy data keys "P" + damaris block number + "_B" + domain number
#  The block number is the source of the data (i.e. the Damaris client number)
#  The domain number is the result of multiple calls to damaris_write_block()
#  or 0 if only damaris_write() API is used.
# 
# N.B. Only the data for the current iteration is available - and it is Read Only. 
#      If it is needed later it needs to be saved somehow and re-read on the 
#      next iteration. When connected to a Dask scheduler then the data can be saved on 
#      the distributed workers.


 
def main(DD):
    from mpi4py import MPI
    import dask.array as da
    import time
    from os import path
    from dask.distributed import Client, TimeoutError, Lock, Variable
        
    try:
        # pass                   # use this to skip whole block
        comm = MPI.COMM_WORLD
        rank = comm.Get_rank()
        
        # keys = list(DD.keys())
        # print(keys)
        # These two dictionaries are set up in the PyAction constructor 
        # and are static.
        damaris_dict = DD['damaris_env']
        dask_dict    = DD['dask_env']
        # This third dictionary is set up in PyAction::PassDataToPython() and is 
        # typically different each iteration.
        iter_dict    = DD['iteration_data']   
        # This is the iteration value the from Damaris perspective
        # i.e. It is the number of times damaris_end_iteration() has been called
        it_str       = str(iter_dict['iteration'])
        
        keys = list(iter_dict.keys())

        for data_key in keys :
            if (data_key != 'iteration'):
                print(data_key)
                   
        # We know the dictionary key as it is the variable name that are in the Damaris XML file
        cube_i =  iter_dict['cube_i']
        # There will be one key and corresponding NumPy array for each block of the variable
        for key in cube_i['numpy_data'].keys() :
            cube_i_numpy = cube_i['numpy_data'][key]
            print('Info from Python: Iteration ', it_str, ' Data found: cube_i[',key,'].sum() = ', cube_i_numpy.sum() )
        
        # The number of block domains depends on use of 'domains' in 
        # the Damaris XML file and use of the damaris_write_block() API
        # There is always at least one per client for which a server looks after.
        block_domains = len( cube_i['block_domains']) 
        # print('for variable cube_i the number of domains (== clients x blocks_per_client) for variable last_iter: ', str(block_domains))
        # print('cube_i[block_domains]: ', cube_i['block_domains'])
        # block_source should always exist - it is a list() of Damaris clients 
        # who sent data to a Damaris server on the current server that is running 
        # this Python code.
        block_source_list = cube_i['block_source']
        print('for variable cube_i block sources list: ', block_source_list)
        
        
        # Do some Dask stuff
        
        # If this file exists (is not a n empty string) then a Dask scheduler was 
        # found (Access has been tested on the Damaris server C++ Python).
        scheduler_file  = dask_dict['dask_scheduler_file']  
        if int(it_str) == 0:
            print('Python INFO: Scheduler file '+ scheduler_file) 
        if (scheduler_file != ""):            
            print("-------------------------------------------------------------------")
            try:      
                client =  Client(scheduler_file=scheduler_file, timeout='2s')
               
                
                
                future  = client.submit( inc, shared_counter, lock )
                res_inc = client.gather(future)  # This blocks until result of all .submit() functions are completed
                
                print('Python INFO: rank: ', str(rank), ' Current value of res_inc: '+str(res_inc)) 
                
                # reset the variable on last iteration
                if int(it_str) == last_iter:
                    shared_counter.set(0)
                           
                client.close()
            except TimeoutError:
                have_dask = False  
            except OSError:
                have_dask = False  

        
        
   
    except KeyError as err: 
        print('KeyError: No damaris data of name: ', err)
    except PermissionError as err:
        print('PermissionError!: ', err)
    except ValueError as err:
        print('Damaris Data is read only!: ', err)
    except UnboundLocalError as err:
        print('Damaris data not assigned!: ', err)
    # finally: is always called.    
    finally:
        pass
        # print('Finally called for iteration: ', DD['iteration'])



if __name__ == '__main__':
    main(DamarisData)
