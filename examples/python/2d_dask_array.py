#!/usr/bin/python3
# Author: Josh Bowden, Inria
# Description: Part of the Damaris examples of using Python integration with Dask distributed
# To run this example, a Dask scheduler needs to be spun up:
# 
#   dask-scheduler --scheduler-file "/home/user/dask_file.json" &
#   dask-worker --scheduler-file "/home/jbowden/dask_file.json"  --nthreads 1 &
#
# Then run the test: 
#
#   mpirun --oversubscribe -np 4 ./2d_dask_array.py
#

import dask.array as da
from dask.distributed import Client
import numpy as np
from mpi4py import MPI
import time
 
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
    p0_full_str = 'client.datasets[\'' + p0_pub_name + '\'][\'' + p0_key + '\'][0]'
    dask_str+=str(p0_full_str)
    t1 = 1
    while t1 < len(myblocks_sorted):
        p1 = myblocks_sorted[t1].offset_tpl
        p1_pub_name = myblocks_sorted[t1].pub_name
        p1_key = myblocks_sorted[t1].P_B_key
        # add first tuple to list
        p1_full_str = 'client.datasets[\'' + p1_pub_name + '\'][\'' + p1_key + '\'][0]'
        dim = tup_len-1
        sepStr , first_elem =  getSeparator2(p0, p1, dim, '',first_elem) 

        dask_str += sepStr + str(p1_full_str)
        t1 = t1 + 1
        p0 = p1
      
    for dim in reversed(range(0, tup_len)):
        dask_str+= ']'
      
    return dask_str
    
# dask-scheduler --scheduler-file "/home/jbowden/dask_file.json" &
# dask-worker --scheduler-file "/home/jbowden/dask_file.json"  --nthreads 1 &
# 
# mpirun -np 4 --oversubscribe ./2d_dask_array.py

class SubBlock:
  def __init__(self,  offset_tpl, pub_name, P_B_key_str):
    self.offset_tpl = offset_tpl
    self.pub_name   = pub_name
    self.P_B_key    = P_B_key_str
    # self.myblock_np = myblock_np
    # print(self.offset_tpl)
  def __repr__(self):
    return repr((self.offset_tpl))



comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

client =  Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s')

mydatadict = {}
mylist = []
# np.ones((3, 4))*2
iteration = 1

         
pub_name ='S'+str(rank) + '_I'+str(iteration)

# Make the data in backwards order, so we have to sort it before passing to returnDaskBlockLayout()
for i in range(4):
    for block in range(4-1,-1,-1):
        if (i % size) == rank:
            myblock = np.ones((3, 4))*i+block
            P_B_key = 'P' + str(rank) +'_B' + str(block)
            block_offset = (i*3, block*4)
            mydatadict[P_B_key]=(myblock,block_offset)
            print('rank ' + str(rank) + ': ', block_offset)
            mylist.append((pub_name, P_B_key, block_offset))

            
mylist = comm.gather(mylist, root=0) # this collects together a list of lists on rank 0
list_merged = []
#for list_itm in mylist:
#    print('list_itm ' + str(list_itm) )
       

    
print('')
if rank == 0:
    # merge into a single list
    for list_itm in mylist:
        list_merged.extend(list_itm)
    print(list_merged)
    print('')
    mylist_sorted = sorted(list_merged, key=lambda tup: tup[2])
    print(mylist_sorted)
    print('')
    # list_merged = comm.scatter(mylist_sorted, root=0)
comm.Barrier()
            

if pub_name in client.datasets.keys():
    client.unpublish_dataset(pub_name)   
client.datasets[pub_name] = mydatadict

comm.Barrier()
time.sleep(0.1)


myblocks_sorted = []
if rank == 0:
    for list_itm in mylist_sorted:
       print('list_itm ' + str(list_itm) )
       pub_name = list_itm[0]
       P_B_key = list_itm[1]
       offset_tpl = list_itm[2]
       myblocks_sorted.append(SubBlock(offset_tpl,pub_name,P_B_key))

    dask_str = returnDaskBlockLayout(myblocks_sorted)
    print(dask_str) 
    exec(dask_str)
    
    x = da.block(data)
    y = x * 2

    print(x.compute())
    print('')
    print(y.compute())
            
else:
    time.sleep(5)

pub_name ='S'+str(rank)  + '_I'+str(iteration)
client.unpublish_dataset(pub_name) 

# data = [
    # [
    # [data_list[0],  data_list[1],  data_list[2],  data_list[3]],
    # [data_list[4],  data_list[5],  data_list[6],  data_list[7]],
    # [data_list[8],  data_list[9],  data_list[10], data_list[11]],
    # [data_list[12], data_list[13], data_list[14], data_list[15]]
    # ]
# ]

# x = da.block(data)

# y = x * 2

# print(x.compute())
# print('')
# print(y.compute())
# print('')
# print(y.chunks)

client.close()

# Shutdown workers and schedulers:
# from dask.distributed import Client
# client =  Client(scheduler_file='/home/jbowden/dask_file.json', timeout='2s')
# client.shutdown()

# P0_B0 = da.from_array(np.zeros((3, 4)) , chunks=(3, 4))
# P1_B0 = da.from_array(np.ones((3, 4))  , chunks=(3, 4))
# P2_B0 = da.from_array(np.ones((3, 4))*2, chunks=(3, 4))
# P3_B0 = da.from_array(np.ones((3, 4))*3, chunks=(3, 4))
# P4_B0 = da.from_array(np.ones((3, 4))*4, chunks=(3, 4))
# P5_B0 = da.from_array(np.ones((3, 4))*5, chunks=(3, 4))
# P6_B0 = da.from_array(np.ones((3, 4))*6, chunks=(3, 4))
# P7_B0 = da.from_array(np.ones((3, 4))*7, chunks=(3, 4))
# P8_B0 = da.from_array(np.ones((3, 4))*8, chunks=(3, 4))
# P9_B0 = da.from_array(np.ones((3, 4))*9, chunks=(3, 4))
# P10_B0 = da.from_array(np.ones((3, 4))*10, chunks=(3, 4))
# P11_B0 = da.from_array(np.ones((3, 4))*11, chunks=(3, 4))
# P12_B0 = da.from_array(np.ones((3, 4))*12, chunks=(3, 4))
# P13_B0 = da.from_array(np.ones((3, 4))*13, chunks=(3, 4))
# P14_B0 = da.from_array(np.ones((3, 4))*14, chunks=(3, 4))
# P15_B0 = da.from_array(np.ones((3, 4))*15, chunks=(3, 4))
 
# data = [
    # [
    # [P0_B0,  P1_B0,  P2_B0,  P3_B0],
    # [P4_B0,  P5_B0,  P6_B0,  P7_B0],
    # [P8_B0,  P9_B0,  P10_B0, P11_B0],
    # [P12_B0, P13_B0, P14_B0, P15_B0]
    # ]
# ]