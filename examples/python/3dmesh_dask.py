# Python code: 3dmesh_py.py

import numpy as np
np.set_printoptions(threshold=np.inf)

# DD (AKA Damaris Data) is a dictionary that has been filled by the 
# Damaris server process with NumPy arraysthat point to the data variables 
# that is exposed in the simulation. The Damaris source file that implements 
# thisis PyAction, found in the src/scripts/ and include/damaris/scripts 
# directories. Damaris <variables> must be exposed to the Python <pyscript>
# XML element by including its name i.e. MyPyAction in the following example:
# 
#    <variable name="cube_i" type="scalar" layout="cells_whd_wf" mesh="mesh" 
#                                                  centering="nodal" script="MyPyAction" />
#
#    <scripts>
#        <pyscript name="MyPyAction" file="3dmesh_py.py" language="python" frequency="1" />
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
    # from mpi4py import MPI
    try:
        # pass
        # comm = MPI.COMM_WORLD
        # rank = comm.Get_rank()
        keys = list(DD.keys())
        # block_domains may not be present.
        # this depends on use of 'domains' in XML file and damaris_write_block() API
        if 'block_domains' in keys:
            block_domains = len( DD['block_domains'])  
            print('The number of domains for variables: ', str(block_domains))
        # block_list should always exist
        block_list = DD['block_source']
        it_str = str(DD['iteration'])
        # if DD['iteration'] == 0 : 
        block_key_base = 'cube_i_int_P' # This is the name of the Dmaris variable in the XML file + data type + '_P'
        for block in block_list :
            block_key = block_key_base + str(block) + '_' +it_str 
            if block_key in DD:
                print("Iteration ",it_str, " Block key ", str(block_key), " Sum =   ", DD[block_key].sum())
            else :
                print ('The key was not found in the DD dictionary! ', block_key)
        
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
