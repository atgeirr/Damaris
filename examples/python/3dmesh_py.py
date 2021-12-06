# Python code: 3dmesh_py.py

import numpy as np
np.set_printoptions(threshold=np.inf)

def main(DamarisDict):
    try:
        print('')
        # if 'np_3d_int_1' in DamarisDict:
            # Assignment to data will fail as in C++ code numpy array was created as 
            # read only using  np::from_data(static_cast<const int *>( )
            # DamarisDict['np_3d_int_1'][1] = 10 
            # print(DamarisDict['np_3d_int_1'])
            # del DamarisDict['np_3d_int_1']
            # del DamarisDict['np_3d_int_1']
            
        # if 'np_3d_int_2' in DamarisDict and DamarisDict['iteration'] == 2:
            # Assignment to data will fail as in C++ code numpy array was created as 
            # read only using  np::from_data(static_cast<const int *>( )
            # DamarisDict['np_3d_int_1'][1] = 10 
            # print(DamarisDict['np_3d_int_2'])
            # del DamarisDict['np_3d_int_2']
        
        # if DamarisDict['iteration'] == 3 :
        print('Hi from iteration ', DamarisDict['iteration'] )
        keys = list(DamarisDict.keys())
        print(keys)
        # del DamarisDict['np_3d_int_1']
        
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
        print('Finally called for iteration: ', DamarisDict['iteration'])
        print('')


if __name__ == '__main__':
    main(DamarisData)
