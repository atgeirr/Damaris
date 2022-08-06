# Python code: stats_3dmesh_dask.py
# Author: Josh Bowden, Inria
# Description: 
# Returns the resulting Average and Variabnce of the simulation data that was computed by Dask.
# 
# Part of the Damaris examples of using Python integration with Dask distributed
# To run this example on Grid5000 (OAR job scheduler), use the script: stats_launcher.sh
# The script is designed to test the damaris4py.damaris_stats class named DaskStats
# 
# N.B. If 4 simulations are launched which each runs 4 iterations, 
#      and each adding a integer 1, 2, 3, or 4 distributed over the 4 simulations 
#      on each iteration, then: 
#      The mean of the first blocks will be 2.5 
#      and the variance for 4 iterations will be 1.333...
from   dask.distributed import Client
from   damaris4py.server import getservercomm
from   damaris4py.dask import damaris_dask
from   damaris4py.dask import damaris_stats
import sys
import numpy as np
np.set_printoptions(threshold=np.inf)

def print_help():
    print(sys.argv[0])
    print('                -f <path to Dask scheduler json file>  ')
    print('                -h  this help message ')


scheduler_file = ''
if len(sys.argv) > 1: 
    if (sys.argv[1] == '-f'):
        scheduler_file = sys.argv[2]
    elif (sys.argv[1] == '-h'):
        print_help()
        sys.exit()
    else:
        print_help()
        sys.exit()
else:
    print_help() 
    sys.exit()
    
try:

    if (scheduler_file != ''):             
        try:      
            client =  Client(scheduler_file=scheduler_file, timeout='2s')
            unique_name_str = 'dask_client_published_str' # Any simulation that uses this can contribute to the running stats
            lock_name_str   = 'dask_lock_str'
            # Creating the DaskStats object on each iteration, it only sets the name for the lock.
            daskstats = damaris_stats.DaskStats( unique_name_str, lock_name_str) 

            (mean, sampleVariance) =  daskstats.return_mean_var_tuple(client, lock_timeout=60)
            count = daskstats.return_count(client, lock_timeout=60)
            print('')
            print('Py results: The damaris_stats count value is: ', count)
            my_tuple = daskstats.get_chunks(client)  # tuple of 1's e.g. (1, 1, 1)
            # These are the averages over the dask blocks, reduces size of outputs
            print(mean.map_blocks(daskstats.compute_block_average, chunks=my_tuple).compute())
            print(sampleVariance.map_blocks(daskstats.compute_block_average, chunks=my_tuple).compute())
            print('')
            # These are the full arrays - they could be big
            # print(mean.compute())
            # print(sampleVariance.compute())
            
            # close the client only:
            client.close()
            
        except TimeoutError as err:
            print(sys.argv[0], '  ERROR: TimeoutError!: ', err) 
        except OSError as err:
            print(sys.argv[0], '  ERROR: OSError!: ', err)
    else:
        print(sys.argv[0], '  INFO: Scheduler file not found:', scheduler_file)
        print_help() 
         
except KeyError as err: 
    print(sys.argv[0], ' ERROR: KeyError: No damaris data of name: ', err)
except PermissionError as err:
    print(sys.argv[0],' ERROR: PermissionError!: ', err)
except ValueError as err:
    print(sys.argv[0],' ERROR: Damaris Data problem!: ', err)
except UnboundLocalError as err:
    print(sys.argv[0],' ERROR: Damaris data not assigned!: ', err)
except NameError as err:
    print(sys.argv[0],' ERROR: NameError: ', err)
# finally: is always called.    
finally:
    pass
