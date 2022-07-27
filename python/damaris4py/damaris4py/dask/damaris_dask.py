__all__ = ['return_scheduler_filename', 'return_iteration', 'return_magic_number', 'return_scalar_from_position', 'return_numpy_array', 'return_dask_array']


def return_separator(p0, p1, dim, instr, first_elem):         
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
            resstr2, first_elem = return_separator(p0, p1, dim-1, instr, first_elem)
            first_elem = True
            resstr += resstr2 + "["           
    return resstr, first_elem


def return_dask_block_layout(mylist_sorted, dask_client_name_str):
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
        sepStr , first_elem =  return_separator(p0_tpl, p1_tpl, dim, '',first_elem) 

        dask_str += sepStr + str(p1_full_str)
        t1 = t1 + 1
        p0_tpl = p1_tpl
      
    for dim in reversed(range(0, tup_len)):
        dask_str+= ']'
      
    return dask_str    
 

def return_scheduler_filename(DamarisData):
    """
        return_scheduler_filename(DamarisData)
        
        DamarisData: Is the dictionary that describes the Damaris data that is created 
                     by the Damaris server processes. It contains a dictionary 'dask_env'
                     that contains the Dask scheduler name.
                     
        If this scheduler_file exists (is not an empty string) then a Dask 
        scheduler was found (This access has been tested on the Damaris server C++ Python).
    """
    dask_dict    = DamarisData['dask_env']
    
    scheduler_file  = dask_dict['dask_scheduler_file']  
    return scheduler_file
    
        
def return_iteration(DamarisData):
    """
        return_iteration(DamarisData)
        
        DamarisData: Is the dictionary that describes the Damaris data that is created 
                     by the Damaris server processes. It contains a dictionary 'iteration_data'
                     that contains meta-data about the current iteration of the simulation. 
                     It is produced by Damaris C++ method PyAction::PassDataToPython()
                     
        Returns the iteration value the from Damaris perspective
        i.e. It is the number of times damaris_end_iteration() has been called.
    """

    iter_dict    = DamarisData['iteration_data']   
    iteration    = iter_dict['iteration']
    return iteration
        
        
        
def return_magic_number(DamarisData):
    """
        return_magic_number(DamarisData)
        
        DamarisData: Is the dictionary that describes the Damaris data that is created 
                     by the Damaris server processes. It contains a dictionary 'damaris_env'
                     that contains meta-data about the current simulation.
                     
        Returns a unique string that identifies the instance of the simulation.
    """
    damaris_dict = DamarisData['damaris_env']
    magic_num    = damaris_dict['simulation_magic_number']
    return magic_num


def return_scalar_from_position(DamarisData, varname, pos=(0), client_rank=0, block_number=0 ):
    """
        return_scalar_from_position(DamarisData, varname, pos=(0), client_rank=0, block_number=0)
        
        DamarisData:   Is the dictionary that describes the Damaris data that is created 
                       by the Damaris server processes. It contains a dictionary 'iteration_data'
                       that contains meta-data about the current simulation.
                     
        varname:      The Damaris variable name - matches names used in the Damaris XML file and used 
                      for damaris_write()
                    
        pos     :     Is the specific index into the NumPy array from which to return the value.
        
        client_rank:  Specifies the Damaris client rank where the NumPy data was produced. 
                      N.B. As data is distributed, only certain Damaris server processes will have 
                      access to particular Damaris client data - they both need to reside on the
                      same host. 
        
        block_number: Specifies the block (or the domain) written by damaris_write_block() or 0 if
                      only damaris_write() has been used.
                     
        Returns single value from the specified NumPy array
    """
    iter_dict    = DamarisData['iteration_data']   
    try:
        if varname in iter_dict.keys():
            array_of_interest =  iter_dict[varname]         
            # Only Process/rank 0 of the Damaris clients (P0) will hold the value
            key = 'P' + str(client_rank) + '_B' + str(block_number)
            if key in array_of_interest['numpy_data'].keys():
                res_val = array_of_interest['numpy_data'][key][pos]
                return res_val
    except IndexError as err:
        print('damaris4py Error: IndexError: ', err)
    except KeyError as err: 
        print('damaris4py: KeyError: No damaris client or block data ', err)

        
def return_numpy_array(DamarisData, varname, client_rank=0, block_number=0 ):
    """
        return_numpy_array(DamarisData, varname, client_rank=0, block_number=0)
        
        DamarisData:   Is the dictionary that describes the Damaris data that is created 
                       by the Damaris server processes. It contains a dictionary 'iteration_data'
                       that contains meta-data about the current simulation.
                     
        varname:      The Damaris variable name - matches names used in the Damaris XML file and used 
                      for damaris_write()
                    
        client_rank:  Specifies the Damaris client rank where the NumPy data was produced. 
                      N.B. As data is distributed, only certain Damaris server processes will have 
                      access to particular Damaris client data - they both need to reside on the
                      same host. 
        
        block_number: Specifies the block (or the domain) written by damaris_write_block() or 0 if
                      only damaris_write() has been used.
                     
        Returns the full NumPy array specified by the client rank and block number
    """
    iter_dict    = DamarisData['iteration_data']   
    try:
        if varname in iter_dict.keys():
            array_of_interest =  iter_dict[varname]         
            key = 'P' + str(client_rank) + '_B' + str(block_number)
            if key in array_of_interest['numpy_data'].keys():
                res_val = array_of_interest['numpy_data'][key]
                return res_val
            else:
                return None
    except KeyError as err: 
        print('damaris4py.ReturnAsNumpy() : KeyError: No damaris client or block data ', err)

           
           
def return_dask_array(DamarisData, client, varname, server_comm, print_array_component=False ):
    """
        return_dask_array(DamarisData, varname, server_comm block_number=0)
        
        DamarisData:   Is the dictionary that describes the Damaris data that is created 
                       by the Damaris server processes. It contains a dictionary 'iteration_data'
                       that contains meta-data about the current simulation.
                     
        varname:      The Damaris variable name - matches names used in the Damaris XML file and used 
                      for damaris_write()
                    
        server_comm:  The simulation communicator collecting Damaris servers.
        
        print_array_component: Specifies whether to print to stdout the array structure or not.
                     
        Returns a created Dask dask.array of the distributed simulation data
    """
    import dask.array as da
    try:
        rank = server_comm.Get_rank()
        iteration = return_iteration(DamarisData)
        # The iter_dict dictionary is set up in PyAction::PassDataToPython() and contains
        # the positional data of where the data comes from and also the NumPy data array of
        # the data section istself.
        iter_dict    = DamarisData['iteration_data']       
        mylist = iter_dict[varname]['sort_list']            
        mylist = server_comm.gather(mylist, root=0) # this collects together a list of lists on rank 0
        mydatadict = iter_dict[varname]['numpy_data']
        
        server_comm.Barrier()
        # for pub_name_key in client.datasets.keys():
        #     client.unpublish_dataset(pub_name_key)   
        global pub_name 
        pub_name = 'S'+str(rank) + '_I'+str(iteration) + '_' + return_magic_number(DamarisData)
        client.datasets[pub_name] = mydatadict
        # print("rank " , rank, "  client.datasets[ " , pub_name, "] was published")
        
        server_comm.Barrier()
        list_merged = []  

        if rank == 0:
            # merge into a single list
            for list_itm in mylist:
                for p_k_bo in list_itm:
                    # p_k_bo is a list of: p_k_bo[0]='string' p_k_bo[1]='string' p_k_bo[3]=list[integers]
                    # and the list[integers] are the offsets into a Damaris array, set using damaris_set_position() API call
                    tlist = [ p_k_bo[0], p_k_bo[1], tuple( p_k_bo[2]) ]
                    ttup = (tuple(tlist))
                    list_merged.append( ttup )

            mylist_sorted = sorted(list_merged, key=lambda tup: tup[2])
            # if iteration == 0:
                # print('ReturnAsDaskArray() Sorted list: \n: ', mylist_sorted)
            
            dask_str = return_dask_block_layout(mylist_sorted, 'client')
            global data
            if print_array_component:
                print('Damaris variable: ', varname, ':\n', dask_str)
            exec("global data; " + dask_str)
            
            # This creates the Dask dask.array
            x = da.block(data)
            return x
        else:
            return None
    except KeyError as err: 
        print('Python ERROR: damaris_dask.ReturnAsDaskArray() KeyError: No damaris data of name: ', err)