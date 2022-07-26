# Python code: stats_3dmesh_dask.py
# Author: Josh Bowden, Inria
# Description: 
# Part of the Damaris examples of using Python integration with Dask distributed
# To run this example on Grid5000 (OAR job scheduler), use the script: stats_launcher.sh
# The script is designed to test the damaris4py.damaris_stats class named DaskStats
# 

import numpy as np 
np.set_printoptions(threshold=np.inf)


# N.B. This file is read by each Damaris server process on each iteration that is 
#      specified by the frequency="" in the <pyscript> XML sttribute.


def main(DD):
    # import dask.array as da
    # import numpy as np
    from   dask.distributed import Client, TimeoutError
    from   damaris4py.server import getservercomm
    from   damaris4py.server import magicnumber_string
    from   damaris4py.dask import damaris_dask
    from   damaris4py.dask import damaris_stats
   
    try:

        damaris_comm = getservercomm()   
        # This is the iteration value the from Damaris perspective
        # i.e. It is the number of times damaris_end_iteration() has been called
        # iteration        = iter_dict['iteration']
        iteration    = damaris_dask.return_iteration(DD)         
        # assert iteration == iteration_dam
        print('Py  Iteration ' + str(iteration) + '  magicnumber_string ' + magicnumber_string())
        damaris_comm.Barrier()

        # Do some Dask stuff - running statistics
        # If this scheduler_file exists (is not an empty string) then a Dask scheduler was 
        # found (and access has been tested on the Damaris server C++ Python side).
        scheduler_file  = damaris_dask.return_scheduler_filename(DD)        
        if iteration == 0:
            print('Python INFO: Scheduler file '+ scheduler_file) 
        if (scheduler_file != ""):                        
            try:      
                client =  Client(scheduler_file=scheduler_file, timeout='2s')
                unique_name_str = 'dask_client_published_str' # Any simulation that uses this can contribute to the running stats
                lock_name_str   = 'dask_lock_str'
                # Creating the DaskStats object on each iteration, as I do not keep them on the Dask scheduler, however I possibly could.
                daskstats = damaris_stats.DaskStats( unique_name_str, lock_name_str) 
                print('Py  Iteration ' + str(iteration) + ':We have the daskstats object')
                # We now create the Dask dask.array
                x =  damaris_dask.return_dask_array(DD, client, 'cube_d', damaris_comm, print_array_component=False )
                print('Py  Iteration ' + str(iteration) + ':We have the dask array returned from return_dask_array()')
                # Only rank 0 returns a dask.array, the others return None
                if (x is not None):
                    print('Py  Iteration ' + str(iteration) + '  calling update()')
                    daskstats.update(x, client, lock_timeout=60)
                    print('Py  Iteration ' + str(iteration) + ':  We have called daskstats.update() using the new data')
                    
                    
                # damaris_comm.Barrier()
                # close the client only:
                client.close()
                
            except TimeoutError as err:
                print('Python ERROR: TimeoutError!: ', err) 
            except OSError as err:
                print('Python ERROR: OSError!: ', err)
        else:
             print('Python INFO: Scheduler file not found:', scheduler_file)
    except KeyError as err: 
        print('Python ERROR: KeyError: No damaris data of name: ', err)
    except PermissionError as err:
        print('Python ERROR: PermissionError!: ', err)
    except ValueError as err:
        print('Python ERROR: Damaris Data problem!: ', err)
    except UnboundLocalError as err:
        print('Python ERROR: Damaris data not assigned!: ', err)
    except NameError as err:
        print('Python ERROR: NameError: ', err)
    # finally: is always called.    
    finally:
        pass



if __name__ == '__main__':
    main(DamarisData)
