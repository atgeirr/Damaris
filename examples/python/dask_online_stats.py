#!/usr/bin/python3
# Author: Josh Bowden, Inria
# Description: Part of the Damaris examples of using Python integration with Dask distributed
# To run this example, a Dask scheduler needs to be spun up:
# 
#   dask-scheduler --scheduler-file "/home/user/dask_file.json" &
#   dask-worker --scheduler-file "/home/jbowden/dask_file.json"  --nthreads 1 &
#

import dask.array as da
import numpy as np
from dask.distributed import Client, Lock, Variable

"""

def create(firstValue):
        count = 0
        mean = da.full_like(firstValue, 0.0)
        M2   = da.full_like(firstValue, 0.0)
        return (count, mean, M2)

    # For a new value newValue, compute the new count, new mean, the new M2.
    # mean accumulates the mean of the entire dataset
    # M2 aggregates the squared distance from the mean
    # count aggregates the number of samples seen so far
    def update(existingAggregate, newValue):
        (count, mean, M2) = existingAggregate
        count += 1
        delta = newValue - mean
        mean += delta / count
        delta2 = newValue - mean
        M2 += delta * delta2
        return (count, mean, M2)

    # Retrieve the mean, variance and sample variance from an aggregate
    def finalize(existingAggregate):
        (count, mean, M2) = existingAggregate
        if count < 2:
            return float("nan")
        else:
            (mean, sampleVariance) = (mean, M2 / (count - 1))
            return (mean, sampleVariance)
            
    def compute_block_average(block):  
        return np.array([np.average(block)])[:, None, None]

    def compute_block_average_of_sqrt(block):  
        return np.array([np.average(np.sqrt(block))])[:, None, None]
        
# Testing
---------    
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
data2 = [
    [
        [P1_B0,  P1_B0,  P1_B0,  P1_B0],
        [P1_B0,  P1_B0,  P1_B0,  P1_B0],
        [P1_B0,  P1_B0,  P1_B0,  P1_B0],
        [P1_B0,  P1_B0,  P1_B0,  P1_B0]
    ]
]

x = da.block(data)

print(x.compute())
print(da.average(x).compute())
existingAggregate = create(x)

for i in range(100):
    y = x + da.random.normal(0.0, 0.1, x.shape)
    existingAggregate = update(existingAggregate, y)

(mean, sampleVariance) = finalize(existingAggregate)

print(mean.map_blocks(compute_block_average, chunks=(1, 1, 1)).compute())
print(sampleVariance.map_blocks(compute_block_average_of_sqrt, chunks=(1, 1, 1)).compute())
print(da.average(da.sqrt(sampleVariance)).compute()))
            """
# from dask.distributed import Client
client =  Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s')


class DaskStats:
    import dask.array as da
    import numpy as np
    """
     A class used to hold a Dask array version of streaming statistics.
     
     Methods
     -------
     
     
    """
    def __init__(self, firstValue):
        self.count = 0    
        self.mean  = da.full_like(firstValue, 0.0)
        self.M2    = da.full_like(firstValue, 0.0)
        
    def update(self, newValue):
        """
        update(newValue)
        
        For a Dask array named newValue, compute the new count, new mean, the new M2.
        
        The newValue array must be the same shape as self.mean and self.M2 (or equivalent)
        mean accumulates the mean at each array point.
        M2 aggregates the squared distance from the mean at each array point.
        count is the total number of samples seen so far.
        
        Currently calls .persit() on the mean and M2 arrays to compute the actual values
        and leave them in distributed memory.
        
        """
        self.count += 1
        delta = newValue - self.mean
        mean += delta / self.count
        delta2 = newValue - self.mean
        self.M2 += delta * delta2
        
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
            return self.mean
            
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
            return sampleVariance       
         
    def return_mean_var_tuple():
        return (return_mean(), return_variance())    

    @staticmethod
    def compute_block_add_rand(block):
        np_data = block + np.random.normal(0.0, 0.1, block.shape) 
        return np_data
        
    @staticmethod
    def compute_block_average(block):  
        return np.array([np.average(block)])[:, None, None]
        
    @staticmethod
    def compute_block_average_of_sqrt(block):  
        return np.array([np.average(np.sqrt(block))])[:, None, None]
        
block_shape = (1, 3, 4)

def TestngDaskStats
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

    # print(x.compute())
    # print(da.average(x).compute())
    daskstats = DaskStats(x)
 
    for i in range(100):
        y = x + da.random.normal(0.0, 0.1, x.shape)
        daskstats.update(y)

    (mean, sampleVariance) = daskstats.return_mean_var_tuple()

    print(mean.map_blocks(daskstats.compute_block_average, chunks=(1, 1, 1)).compute())
    print(sampleVariance.map_blocks(daskstats.compute_block_average_of_sqrt, chunks=(1, 1, 1)).compute())
    # Compare with global derived value
    print(da.average(da.sqrt(sampleVariance)).compute()))
    
    # client.shutdown()













