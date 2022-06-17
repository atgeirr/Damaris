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
#         'sort_data',
#         # 'block_source', 
#         # 'block_domains', 
#         'type_string'
#        ])
# 
# DD['iteration_data']['cube_i']['sort_data']
# sort_data is a list, that can be sorted on (possibly required to be transformed to tuple) data:
#    ['string', 'string', [ <block_offset values> ]]
#   A specific example:
#     ['S0_I1', 'P0_B0', [ 0, 9, 12 ]]
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
    from pydamaris import damaris_comm
    
     
    def getSeparator2(p0, p1, dim, instr, first_elem):         
        resstr = ''
        if p1[dim] > p0[dim]:
            if first_elem == False:
                resstr = instr + ","
            else:
                first_elem = False
                resstr = instr + ","
        elif p1[dim] <= p0[dim]:
            if dim > 0:
                instr += "]\n"
                first_elem = False
                resstr2, first_elem = getSeparator2(p0, p1, dim-1, instr, first_elem)
                first_elem = True
                resstr += resstr2 + "["           
        return resstr, first_elem
      

    def returnDaskBlockLayout(myblocks_sorted):
        tup_len = len(myblocks_sorted[0].offset_tpl)
        
        dask_str='data = '
        for dim in reversed(range(0, tup_len)):
            dask_str+= '[' 
        
        # initialize inputs
        first_elem = False 
        p0 = myblocks_sorted[0].offset_tpl
        p0_pub_name = myblocks_sorted[0].pub_name
        p0_key = myblocks_sorted[0].P_B_key
        # add first tuple to list
        p0_full_str = 'client.datasets[\'' + p0_pub_name + '\'][\'' + p0_key + '\']'
        dask_str+=str(p0_full_str)
        t1 = 1
        while t1 < len(myblocks_sorted):
            p1 = myblocks_sorted[t1].offset_tpl
            p1_pub_name = myblocks_sorted[t1].pub_name
            p1_key = myblocks_sorted[t1].P_B_key
            # add first tuple to list
            p1_full_str = 'client.datasets[\'' + p1_pub_name + '\'][\'' + p1_key + '\']'
            dim = tup_len-1
            sepStr , first_elem =  getSeparator2(p0, p1, dim, '',first_elem) 

            dask_str += sepStr + str(p1_full_str)
            t1 = t1 + 1
            p0 = p1
          
        for dim in reversed(range(0, tup_len)):
            dask_str+= ']'
          
        return dask_str
     
 
    def returnDaskBlockLayoutList(mylist_sorted, dask_client_name_str):
        tup_len = len(myblocks_sorted[2])
        
        dask_str='data = '
        for dim in reversed(range(0, tup_len)):
            dask_str+= '[' 
        
        # initialize inputs
        first_elem = False         
        p0_pub_name = myblocks_sorted[0]
        p0_key = myblocks_sorted[1]
        p0_tpl = myblocks_sorted[2]
        # add first tuple to list
        p0_full_str = dask_client_name_str + '.datasets[\'' + p0_pub_name + '\'][\'' + p0_key + '\']'
        dask_str+=str(p0_full_str)
        t1 = 1
        while t1 < len(myblocks_sorted):            
            p1_pub_name = myblocks_sorted[0].pub_name
            p1_key = myblocks_sorted[1].P_B_key
            p1_tpl = myblocks_sorted[2].offset_tpl
            # add first tuple to list
            p1_full_str = dask_client_name_str + '.datasets[\'' + p1_pub_name + '\'][\'' + p1_key + '\']'
            dim = tup_len-1
            sepStr , first_elem =  getSeparator2(p0_tpl, p1_tpl, dim, '',first_elem) 

            dask_str += sepStr + str(p1_full_str)
            t1 = t1 + 1
            p0_tpl = p1_tpl
          
        for dim in reversed(range(0, tup_len)):
            dask_str+= ']'
          
        return dask_str    
    class SubBlock:
        def __init__(self,  offset_tpl, pub_name, P_B_key_str):
            self.offset_tpl = offset_tpl
            self.pub_name   = pub_name
            self.P_B_key    = P_B_key_str
            # self.myblock_np = myblock_np
            # print(self.offset_tpl)
        def __repr__(self):
            return repr((self.offset_tpl))


    try:
        # pass
        # comm = MPI.COMM_WORLD
        # rank = comm.Get_rank()
        damaris_comm = damaris_comm()
        rank = damaris_comm.Get_rank()
        size = damaris_comm.Get_size()
        print('rank: ', rank, ' of : ', size)    
        
        
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
        iteration    = iter_dict['iteration']
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
        

        # Do some Dask stuff
        # If this scheduler_file exists (is not an empty string) then a Dask scheduler was 
        # found (Access has been tested on the Damaris server C++ Python).
        scheduler_file  = dask_dict['dask_scheduler_file']  
        if int(it_str) == 0:
            print('Python INFO: Scheduler file '+ scheduler_file) 
        if (scheduler_file != ""):            
            print("-------------------------------------------------------------------")
            try:      
                client =  Client(scheduler_file=scheduler_file, timeout='2s')
                # time.sleep(1.0)
                
                mylist = iter_dict['cube_i']['sort_list']            
                mylist = damaris_comm.gather(mylist, root=0) # this collects together a list of lists on rank 0
                mydatadict = iter_dict['cube_i']['numpy_data']
                if rank == 0:
                    print(mylist)
               
                for pub_name_key in client.datasets.keys():
                     client.unpublish_dataset(pub_name_key)   
                global pub_name 
                pub_name = 'S'+str(rank) + '_I'+str(iteration)
                client.datasets[pub_name] = mydatadict
                print("rank " , rank, "  client.datasets[ " , pub_name, "] was published")
                time.sleep(0.5)
                
                # time.sleep(2)
                damaris_comm.Barrier()
                
                list_merged = []  
                myblocks_sorted = []
                print('')
                if rank == 0:
                    # merge into a single list
                    for list_itm in mylist:
                        for p_k_bo in list_itm:
                            tlist = [ p_k_bo[0],p_k_bo[1],tuple(p_k_bo[2]) ]
                            ttup = (tuple(tlist))
                            list_merged.append( ttup )
                    print('list_merged:')        
                    print(list_merged)
                    print('')
                    mylist_sorted = sorted(list_merged, key=lambda tup: tup[2])
                    print(mylist_sorted)
                    print('')
                    
                    for list_itm in mylist_sorted:
                       print('list_itm ' + str(list_itm) )
                       pub_name_tmp = list_itm[0]
                       P_B_key = list_itm[1]
                       offset_tpl = list_itm[2]
                       myblocks_sorted.append(SubBlock(offset_tpl,pub_name_tmp,P_B_key))

                    dask_str = returnDaskBlockLayoutList(mylist_sorted, 'client')
                    print(dask_str) 
                    global data 
                    exec("global data; " + dask_str)
                    
                    x = da.block(data)
                    y = x * 2

                    print(x.compute())
                    print('')
                    print(y.compute())

                damaris_comm.Barrier()
                
                # pub_name ='S'+str(rank)  + '_I'+str(iteration)
                client.unpublish_dataset(pub_name) 
                print("rank " , rank, "  client.datasets[ " , pub_name, "] was unpublished")
                client.close()
                
            except TimeoutError:
                have_dask = False  
            except OSError:
                have_dask = False  

    except KeyError as err: 
        print('Python Script KeyError: No damaris data of name: ', err)
    except PermissionError as err:
        print('Python Script PermissionError!: ', err)
    except ValueError as err:
        print('Python Script Damaris Data is read only!: ', err)
    except UnboundLocalError as err:
        print('Python Script Damaris data not assigned!: ', err)
    except NameError as err:
        print('Python Script NameError: ', err)
    # finally: is always called.    
    finally:
        pass
        # print('Finally called for iteration: ', DD['iteration'])



if __name__ == '__main__':
    main(DamarisData)
