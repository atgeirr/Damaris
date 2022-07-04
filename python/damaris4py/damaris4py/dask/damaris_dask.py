  
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


def returnDaskBlockLayoutList(mylist_sorted, dask_client_name_str):
    tup_len = len(mylist_sorted[2])
    
    dask_str='data = '
    for dim in reversed(range(0, tup_len)):
        dask_str+= '[' 
    
    # initialize inputs
    first_elem = False         
    p0_pub_name = mylist_sorted[0][0]
    p0_key = mylist_sorted[0][1]
    p0_tpl = mylist_sorted[0][2]
    # add first tuple to list
    p0_full_str = dask_client_name_str + '.datasets[\'' + p0_pub_name + '\'][\'' + p0_key + '\']'
    dask_str+=str(p0_full_str)
    t1 = 1
    while t1 < len(mylist_sorted):            
        p1_pub_name = mylist_sorted[t1][0]
        p1_key = mylist_sorted[t1][1]
        p1_tpl = mylist_sorted[t1][2]
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
 

def CheckScheduler()
    if 'DamarisData' in locals():
        dask_dict    = DamarisData['dask_env']
        # If this scheduler_file exists (is not an empty string) then a Dask scheduler was 
        # found (Access has been tested on the Damaris server C++ Python).
        scheduler_file  = dask_dict['dask_scheduler_file']  
        return scheduler_file
    else:
        print('damaris4py: ERROR: DamarisData is not in locals()')
        
def ReturnIteration()
    if 'DamarisData' in locals():
        # This third dictionary is set up in PyAction::PassDataToPython() and is 
        # typically different each iteration.
        iter_dict    = DamarisData['iteration_data']   
        # This is the iteration value the from Damaris perspective
        # i.e. It is the number of times damaris_end_iteration() has been called
        iteration    = iter_dict['iteration']
        return iteration
    else:
        print('damaris4py: ERROR: DamarisData is not in locals()')
        
def ReturnMagicNumber()
    if 'DamarisData' in locals():

        damaris_dict = DamarisData['damaris_env']
        magic_num    = damaris_dict['simulation_magic_number']
        return magic_num
    else:
        print('damaris4py: ERROR: DamarisData is not in locals()')
        return None

def ReturnAsScalarFromClient(varname, pos=(0), client_rank=0, block_number=0 )
    if 'DamarisData' in locals():
        # This third dictionary is set up in PyAction::PassDataToPython() and is 
        # typically different each iteration.
        iter_dict    = DamarisData['iteration_data']   
        # The value passed through by Damaris clients is to be compared with the value computed by Dask.
        # It is an array of a single value, which changes each iteration and is
        # the sum of all values in the whole cube_i dataset in distributed memory.
        # The sum is only available on client rank 0
        try:
            if varname in iter_dict.keys():
                array_of_interest =  iter_dict[varname]         
                # Only Process/rank 0 of the Damaris clients (P0) will hold the value
                key = 'P' + str(client_rank) + _B + str(block_number)
                if key in array_of_interest['numpy_data'].keys():
                    res_val = array_sum['numpy_data'][key][pos]
                    return res_val
        except IndexError as err:
            print('damaris4py Error: IndexError: ', err)
        except KeyError as err: 
            print('damaris4py: KeyError: No damaris client or block data ', err)
    else:
        print('damaris4py: ERROR: DamarisData is not in locals()')
        return None
        
def ReturnAsNumpy(varname )
    if 'DamarisData' in locals():
        # This third dictionary is set up in PyAction::PassDataToPython() and is 
        # typically different each iteration.
        iter_dict    = DamarisData['iteration_data']   
        # The value passed through by Damaris clients is to be compared with the value computed by Dask.
        # It is an array of a single value, which changes each iteration and is
        # the sum of all values in the whole cube_i dataset in distributed memory.
        # The sum is only available on client rank 0
        try:
            if varname in iter_dict.keys():
                array_of_interest =  iter_dict[varname]         
                # Only Process/rank 0 of the Damaris clients (P0) will hold the value
                res_val = array_sum['numpy_data'][key]
                return res_val
        except KeyError as err: 
            print('damaris4py: KeyError: No damaris client or block data ', err)
    else:
        print('damaris4py: ERROR: DamarisData is not in locals()')
        return None
           
           
def ReturnAsDaskArray(varname, server_comm, print_array_component=False )
    if 'DamarisData' in locals():
        try:
            rank = damaris_comm.Get_rank()
            iteration = ReturnIteration()
            # This third dictionary is set up in PyAction::PassDataToPython() and is 
            # typically different each iteration.
            iter_dict    = DamarisData['iteration_data']       
            mylist = iter_dict[varname]['sort_list']            
            mylist = server_comm.gather(mylist, root=0) # this collects together a list of lists on rank 0
            mydatadict = iter_dict[varname]['numpy_data']
            
             
            server_comm.Barrier()
            # for pub_name_key in client.datasets.keys():
            #     client.unpublish_dataset(pub_name_key)   
            global pub_name 
            pub_name = 'S'+str(rank) + '_I'+str(iteration) + '_' + ReturnMagicNumber()
            client.datasets[pub_name] = mydatadict
            # print("rank " , rank, "  client.datasets[ " , pub_name, "] was published")
            
            server_comm.Barrier()
            list_merged = []  

            if rank == 0:
                # merge into a single list
                for list_itm in mylist:
                    for p_k_bo in list_itm:
                        # p_k_bo is a list of: p_k_bo[0]='string' p_k_bo[1]='string'  p_k_bo[3]=list[integers]
                        # and the list[integers] are the offsets into a Damaris array, set using damaris_set_position() API call
                        tlist = [ p_k_bo[0], p_k_bo[1], tuple( p_k_bo[2]) ]
                        ttup = (tuple(tlist))
                        list_merged.append( ttup )

                # print('')
                mylist_sorted = sorted(list_merged, key=lambda tup: tup[2])
                if iteration == 0:
                    print(mylist_sorted)
                
                dask_str = returnDaskBlockLayoutList(mylist_sorted, 'client')
                global data
                if print_array_component:
                    print('Damaris variable: ', varname, ':\n', data)
                exec("global data; " + dask_str)
                
                
                # We now create the Dask dask.array
                x = da.block(data)
                return x
        except KeyError as err: 
            print('Python INFO: KeyError: No damaris data of name: ', err)        x = da.block(data)