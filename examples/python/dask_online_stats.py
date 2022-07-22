#!/usr/bin/python3
# Author: Josh Bowden, Inria
# Usage:
# > ./dask_online_stats <float> | -u | -s | -r |
# <float> a value to add to the data
#  -u     Unpublish datasets 
#  -s     Shutdown dask scheduler 
#  -r    [num] Start Dask Scheduler with num workers (default 2)  
#
# Description: 
#  Test code for streaming statistics using Dask, based on Welford's online algorithm.
#  (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#:~:text=to%20a%20degree.-,Welford%27s%20online%20algorithm,-%5Bedit%5D)
#
#  Part of the Damaris examples of using Python integration with Dask distributed
#  To run this example, a Dask scheduler needs to be spun up:
# 
#

import dask.array as da
import numpy as np
from dask.distributed import Client
import sys


class DaskStats:
    import dask.array as da
    import numpy as np
    """
     A class used to hold a Dask array version of streaming statistics.
     
     Methods
     -------
     
     
    """
    def __init__(self, array_of_shape ):
        """
        Constructor
        
        array_of_shape: is a dask.array, from which we will create the mean and M2 arrays
        of the same shape - including the block layout of a distributed array. All values 
        are initialised to 0.0
        
        """
        self.count = 0    
        self.mean  = da.full_like(array_of_shape, dtype=np.float64)
        self.M2    = da.full_like(array_of_shape, dtype=np.float64)
        
    def update(self, newValue):
        """
        update(newValue)
        
        For a Dask array named newValue, compute the new count, new mean, the new M2.
        
        The newValue array must be the same shape as self.mean and self.M2 (or equivalent)
        mean accumulates the mean at each array point.
        M2 aggregates the squared distance from the mean at each array point.
        count is the total number of samples seen so far.
        
        Currently calls .persist() on the mean and M2 arrays to compute the actual values
        and leave them in distributed memory.
        
        """
        assert newValue.shape == self.mean.shape
        
        print('update() self.count = ', self.count)
        
        self.count += 1
        delta = newValue - self.mean
        delta.persist()
        self.mean += delta / self.count
        self.mean.persist()
        delta2 = newValue - self.mean
        self.M2 += delta * delta2
        
        # self.mean.persist()
        self.M2.persist()
        
        
    def save_on_dask(self, client, unique_name_str ):
        """
        save_on_dask
        
        client:  A Dask distributed client
        unique_name_str: Key to set so we can find correct dataset
        
        Saves our data to the Dask server so we can retirve it from another client
        
        """
        if unique_name_str+'_count' in client.list_datasets():
            client.unpublish_dataset(unique_name_str+'_count')    
            client.unpublish_dataset(unique_name_str+'_mean')  
            client.unpublish_dataset(unique_name_str+'_M2')   
            
        client.datasets[unique_name_str+'_count'] = self.count    
        client.datasets[unique_name_str+'_mean']  = self.mean  
        client.datasets[unique_name_str+'_M2']    = self.M2  
    
    
    def retrieve_from_dask(self, client, unique_name_str ):
        """
        retrieve_from_dask
        
        client:  A Dask distributed client
        unique_name_str: Key to set so we can find correct dataset
        
        Retrieve the saved data (saved by save_on_dask())
        
        """
        
        # Do not use this assert as we may not know the shape
        # assert self.mean.shape == client.datasets[unique_name_str+'_mean'].shape
        
        self.count = client.datasets[unique_name_str+'_count']
        self.mean  = client.datasets[unique_name_str+'_mean']
        self.M2    = client.datasets[unique_name_str+'_M2'] 
        self.mean.persist()
        self.M2.persist()

    def return_mean(self):
        """
        return_mean
        
        Retrieve the Dask array 'future' self.mean
        """
        if self.count < 2:
            return da.full_like(self.mean, float("nan")) 
        else:
            return self.mean.persist()
            
            
    def return_variance(self):
        """
        return_variance
        
        Calculate and return the Dask array sample variance 
        
        Computed as: self.M2 / (self.count - 1)
        
        Returns a 'future', so to get the values we must .compute() or .persist()
        """
        if self.count < 2:
            return da.full_like(self.mean, float("nan")) 
        else:
            sampleVariance = ( self.M2 / (self.count - 1))
            return sampleVariance.persist()      
         
         
    def return_mean_var_tuple(self):
        return (self.return_mean(), self.return_variance())    


    @staticmethod
    def compute_block_add_rand(block, mean=0.0, std=0.1):
        """
        compute_block_add_rand(block, mean, std)
        
        mean : the mean of the distribution to draw random values from
        std  : the standard deviation of the distribution to draw random values from
        
        
        Usefull for da.map_blocks(daskstats.compute_block_add_rand, chunks=(1, 1, 1)).compute())
        Result:
        End up with a dask array with random values added.
        """
        
        np_data = block + np.random.normal(mean, std, np.shape(block)) 
        return np_data
        

    def compute_block_average(self, block):  
        """
        compute_block_average(block)
        
        Usefull for da.map_blocks(daskstats.compute_block_average, chunks=(1, 1, 1)).compute())
        Result:
        End up with a Numpy array of <number of chunks in the dask array> values
        """
        shape_len = len(block.shape)
        global res 
        exec_str = 'global res; res = np.array([np.average(block)])' + self.get_array_as_string(shape_len)
        exec(exec_str)
        # res = np.array([np.average(block)])[:, None, None]
        return(res)
        

    def compute_block_average_of_sqrt(self, block):  
        """
        compute_block_average_of_sqrt(block)
        
        Usefull for da.map_blocks(daskstats.compute_block_average_of_sqrt, chunks=(1, 1, 1)).compute())
        
        Result:
        End up with a Numpy array of <number of chunks in the dask array> values,
        each value is the ave(sqrt(original data)), sqrt() is per element and ave() is a reduction.
        """
        shape_len = len(block.shape)
        global res 
        exec_str = 'global res; res = np.array([np.average(np.sqrt(block))])' + self.get_array_as_string(shape_len)
        exec(exec_str)
        # res = np.array([np.average(np.sqrt(block))])[:, None, None]
        return(res)
    
    
    @staticmethod    
    def get_array_as_string( shape_len ):
        """
        get_array_as_string
        
        Returns a string enclosed in [], with contents :, None,... with the number of None, added
        being the length of the number of dimensions of the input array.
        
        e.g. if array has 3 dims then returns string  '[:, None, None]'
        
        This is useful for compute_block routines
        
        """
        res_str = '[: '
        for i in range(shape_len-1):
            res_str += ', None'
        res_str += ']'
        return res_str


    @staticmethod    
    def get_chunks( dask_array ):
        """
        get_chunks
        
        Returns a tuple of 1's the length of the number of dimensions of the input array.
        
        e.g. if array has 3 dims then returns (1, 1, 1). 
        
        This is useful for the chunks= attribute of map_blocks()
        
        """
        tuplist = list()
        for i in range(len(dask_array.shape)):
            tuplist.append(1)
        chunks_tup=tuple(tuplist)
        return chunks_tup
    
    

      
block_shape = (1, 3, 4)


P0_B0 = np.zeros(block_shape )
P1_B0 = np.ones(block_shape  )
P2_B0 = np.ones(block_shape)*2
P3_B0 = np.ones(block_shape)*3
P4_B0 = np.ones(block_shape)*4
P5_B0 = np.ones(block_shape)*5
P6_B0 = np.ones(block_shape)*6
P7_B0 = np.ones(block_shape)*7
P8_B0 = np.ones(block_shape)*8
P9_B0 = np.ones(block_shape)*9
P10_B0 = np.ones(block_shape)*10
P11_B0 = np.ones(block_shape)*11
P12_B0 = np.ones(block_shape)*12
P13_B0 = np.ones(block_shape)*13
P14_B0 = np.ones(block_shape)*14
P15_B0 = np.ones(block_shape)*15

data = [
    [
        [P0_B0,  P1_B0,  P2_B0,  P3_B0],
        [P4_B0,  P5_B0,  P6_B0,  P7_B0],
        [P8_B0,  P9_B0,  P10_B0, P11_B0],
        [P12_B0, P13_B0, P14_B0, P15_B0]
    ]
]

x = da.block(data)

def compute_block_average(block):  
    return np.array([np.average(block)])[:, None, None]

def compute_block_average_of_sqrt(block):  
    return np.array([np.average(np.sqrt(block))])[:, None, None]

# from dask.distributed import Client
def print_help()
    print(sys.argv[1], '  <float> ')
    print(sys.argv[1], '  -u  Unpublish datasets ')
    print(sys.argv[1], '  -s  Shutdown dask scheduler ')
    print(sys.argv[1], '  -r  [num] Restart Dask Scheduler with num workers (default 2)  ')

unpublish = False
shutdown = False
restart = False
how_many_workers = 2
if len(sys.argv) > 1: 
    if (sys.argv[1] == '-u'):
        print('unpublish = True')
        unpublish = True
    elif (sys.argv[1] == '-s'):
        print('shutdown = True')
        shutdown = True
    elif (sys.argv[1] == '-r'):
        print('restart = True')
        restart = True
        if len(sys. argv) > 2:
            how_many_workers = int(sys.argv[2])
    elif (sys.argv[1] == '-h'):
        print_help()
        sys.exit()
    else:
        myval = float(sys.argv[1])
else:
    print_help() 


    
if restart == True :
    import subprocess, time
    print('Restarting down the Dask Scheduler and workers!')
    #client.shutdown()
    subprocess.Popen(['dask-scheduler', '--scheduler-file', '/home/jbowden/dask_file.json'])
    time.sleep(1.0)
    # dask-scheduler --scheduler-file "/home/$USER/dask_file.json" &
    for i in range(how_many_workers):
        subprocess.Popen(['dask-worker', '--scheduler-file', '/home/jbowden/dask_file.json',  '--nthreads', '1'])

client =  Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s')
print(client.list_datasets())

if unpublish == True :
    for ds  in client.list_datasets():
        print('Unpublishing Daskstats:', ds)
        client.unpublish_dataset(ds)
    client.close()
    sys.exit()
    
if shutdown == True :
    print('Shuting down the Dask Scheduler and workers!')
    client.shutdown()
    sys.exit()
    
    
if 'unique_name1' not in client.list_datasets():
    print('Creating Daskstats:')
    daskstats = DaskStats(x)
    client.datasets['unique_name1'] = 'unique_name1'
else:
    print('Retrieving Daskstats:')
    daskstats = DaskStats(x)
    daskstats.retrieve_from_dask(client, 'unique_name1' )
         

# Update the dataset
for i in range(10):
    y = x + myval# da.random.normal(0.0, 0.1, x.shape)
    daskstats.update(y)


print('The current mean and stddev values are:')
(mean, sampleVariance) = daskstats.return_mean_var_tuple()

my_tuple = daskstats.get_chunks(x)  # tuple of 1's e.g. (1, 1, 1)
print(mean.map_blocks(daskstats.compute_block_average, chunks=(1, 1, 1)).compute())
print(sampleVariance.map_blocks(daskstats.compute_block_average_of_sqrt, chunks=my_tuple).compute())

# Compare with global derived value
print(da.average(da.sqrt(sampleVariance)).compute())

sys.stdout.flush()
daskstats.save_on_dask(client, 'unique_name1' )

client.close()













