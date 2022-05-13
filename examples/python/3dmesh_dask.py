# Python code: 3dmesh_py.py

import numpy as np
np.set_printoptions(threshold=np.inf)

# DD (AKA Damaris Data) is a dictionary that has been filled by the 
# Damaris server process with NumPy arrays that point to the data variables 
# that is exposed in the simulation. The Damaris source file that implements 
# thisis PyAction, found in the src/scripts/ and include/damaris/scripts 
# directories. Damaris <variables> must be exposed to the Python <pyscript>
# XML element by including its name i.e. MyPyAction in the following example:
# 
#    <variable name="cube_i" type="scalar" layout="cells_whd_wf" mesh="mesh" 
#                                                  centering="nodal" script="MyPyAction" />
#
#    <scripts>
#       <pyscript name="MyPyAction" file="3dmesh_py.py" language="python" frequency="1"
#            scheduler-file="/home/jbowden/dask_file.json" nthreads="1" keep-workers="no" />
#    </scripts>
#
# The server processes also present some metadata so that we can create key names to access the data
# DD['iteration']     - The current simulation iteration
# DD['block_source']  - The block source - each damaris client is refered to as a source, 
#                        and a damaris server will process multiple clients.
# DD['block_domains'] - A single simulation iteration might write multiple blocks of data 
#                        (if domains in the simulation XML file  > 1)
#
# N.B. Only the data for the current iteration is available - and it is Read Only. 
#      If it is needed later it needs to be saved (pickleed maybe) and re-read on the 
#      next iteration. When connected to a Dask scheduler then the data can be saved on 
#      the distributed workers.
#      Also, this file is read by each Damaris server process on each iteration that is 
#      specified by the frequency="" XML sttribute.

   
 
def main(DD):
    from mpi4py import MPI
    import dask.array as da
    import time
    from os import path
    from dask.distributed import Client, TimeoutError
    def inc(x: int) -> int:
        return x + 1
    try:
        
        # pass
        comm = MPI.COMM_WORLD
        rank = comm.Get_rank()
        keys = list(DD.keys())
        
        # if 'dask_scheduler_file' in keys: 
        # block_domains may not be present.
        # this depends on use of 'domains' in XML file and damaris_write_block() API
        if 'block_domains' in keys:
            block_domains = len( DD['block_domains'])  
            print('The number of domains for variables: ', str(block_domains))
        # block_list should always exist
        block_list = DD['block_source']


        it_str = str(DD['iteration'])
        # if (int(it_str) == 0): 
        print(keys)
            
        block_key_base = 'last_iter_int_P' # This is the name of the Dmaris variable in the XML file + data type + '_P'
        for block in block_list :
            block_key = block_key_base + str(block) + '_' + it_str 
            if block_key in DD:
                last_iter = DD[block_key]
                # print('**LAST ITERATION **: ' , str(last_iter))  
            else :
                print ('The key was not found in the DD dictionary! ', block_key)
        # last_iter = str(DD['last_iter_int_P'+])
        # print("** LAST ITERATION **",last_iter)
        
        dask_exists_int = DD['dask_scheduler_exists']   # == 1 if it exists
        if (dask_exists_int == 1):
            scheduler_file  = DD['dask_scheduler_file']
            print("-------------------------------------------------------------------")
            print("Iteration ",it_str, " DD['dask_scheduler_file'] ;  ", scheduler_file)
            try:
                print("Iteration ",it_str, "scheduler: ", scheduler_file, "  Calling time(): p1a", time.time())
                # if ( path.exists(scheduler_file) ):
                client = Client(scheduler_file=scheduler_file, timeout='2s')
                if 'res_inc_a_'+str(rank) in client.datasets:
                    res_a = client.datasets['res_inc_a_'+str(rank)]
                else:
                    res_a = 2
                print("Iteration ",it_str, "  Our current res_a value is: ", res_a)
                a = client.submit(inc, res_a)
                res_inc_a = client.gather(a) 
                print("Iteration ",it_str, "  Our current inc value is: ", res_inc_a)
                try:
                    client.unpublish_dataset('res_inc_a_'+str(rank))
                    print('Unpublishing res_inc_a_'+str(rank)) 
                    client.datasets['res_inc_a_'+str(rank)]= res_inc_a  
                except KeyError: 
                    pass
                # print("Iteration ",it_str, "Calling time() p2b:", time.time())
                #if (int(it_str) == last_iter-1):
                           
                client.close()
            except TimeoutError:
                have_dask = False  
            except OSError:
                have_dask = False  

        
        

        # if DD['iteration'] == 0 : 
  #      block_key_base = 'cube_i_int_P' # This is the name of the Dmaris variable in the XML file + data type + '_P'
  #      for block in block_list :
  #          block_key = block_key_base + str(block) + '_' +it_str 
  #          if block_key in DD:
  #              print("Iteration ",it_str, " Block key ", str(block_key), " Sum =   ", DD[block_key].sum())
  #          else :
  #             print ('The key was not found in the DD dictionary! ', block_key)
        
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
        #print('Finally called for iteration: ', DD['iteration'])
        #print('')


if __name__ == '__main__':
    main(DamarisData)
