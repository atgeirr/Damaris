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


# from dask.distributed import Client
client =  Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s')

from copy import copy
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
        (mean, variance, sampleVariance) = (mean, M2 / count, M2 / (count - 1))
        return (mean, variance, sampleVariance)
     
     
# Using to sum up elements in each block, see: 
#https://stackoverflow.com/questions/40092808/compute-sum-of-the-elements-in-a-chunk-of-a-dask-array
#def compute_block_sum(block):
#    return np.array([np.sum(block)])[:, None, None]

def compute_block_add_rand(block):
    np_data = block - np.random.rand(block.shape) 
    return np_data

client.shutdown()

P0_B0 = np.zeros((3, 4)) )
P1_B0 = np.ones((3, 4))  )
P2_B0 = np.ones((3, 4))*2)
P3_B0 = np.ones((3, 4))*3)
P4_B0 = np.ones((3, 4))*4)
P5_B0 = np.ones((3, 4))*5)
P6_B0 = np.ones((3, 4))*6)
P7_B0 = np.ones((3, 4))*7)
P8_B0 = np.ones((3, 4))*8)
P9_B0 = np.ones((3, 4))*9)
P10_B0 = np.ones((3, 4))*10)
P11_B0 = np.ones((3, 4))*11)
P12_B0 = np.ones((3, 4))*12)
P13_B0 = np.ones((3, 4))*13)
P14_B0 = np.ones((3, 4))*14)
P15_B0 = np.ones((3, 4))*15)
 
data = [
    [
    [P0_B0,  P1_B0,  P2_B0,  P3_B0],
    [P4_B0,  P5_B0,  P6_B0,  P7_B0],
    [P8_B0,  P9_B0,  P10_B0, P11_B0],
    [P12_B0, P13_B0, P14_B0, P15_B0]
    ]
]


x = da.block(data)
# y = da.full_like(x, 12.3)
# y = x.map_blocks(compute_block_add_rand, chunks=(3, 4)).persist()
y = x.map_blocks(compute_block_add_rand, chunks=(3, 4)).compute()

print(y)















