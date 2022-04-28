# Python code: 3dmesh_py.py

import numpy as np
np.set_printoptions(threshold=np.inf)

# DD is a dictionary that has been filled by the Damaris server process with NumPy arrays
# that point to the data that is exposed in the simulation.
# The server processes also present some metadata so that we can create key names to access the data
# DD['iteration']   - The current simulation iteration
# DD['block_source'] - The block source - each damaris client is refered to as a source, and a damaris server will process multiple clients.
# DD['block_domains'] - A single simulation interation might write multiple blocks of data (if domains in the simulation XML file  > 1)
def main(DD):
    #from mpi4py import MPI
    try:
        # pass
        # print('')
        # if 'np_3d_int_1' in DD:
            # Assignment to data will fail as in C++ code numpy array was created as 
            # read only using  np::from_data(static_cast<const int *>( )
            # DD['np_3d_int_1'][1] = 10 
            # print(DD['np_3d_int_1'])
        #comm = MPI.COMM_WORLD
        #rank = comm.Get_rank()
        keys = list(DD.keys())
        block_domains = len( DD['block_domains'])  # if each block has multiple domains then this will not be empty
        print('The number of domains for variables: ', str(block_domains))
        block_list = DD['block_source']
        it_str = str(DD['iteration'])
        # if DD['iteration'] == 0 : 
        block_key_base = 'cube_i_int_P'
        for block in block_list :
            block_key = block_key_base + str(block) + '_' +it_str 
            if block_key in DD:
                print('block_key', block_key)
                print(DD[block_key].shape)
                print(DD[block_key].sum())
            else :
                print ('The key was not found in the DD dictionary! ', block_key)
            #
        
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
