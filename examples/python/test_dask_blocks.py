#!/usr/bin/env python3

# This file is part of the damaris gitlab repo (possibly damaris-development)
# python3 -m pip install dask[complete]
#
# 
# 
# git clone --recursive https://github.com/llnl/conduit.git
# cd conduit
# env ENABLE_MPI=ON HDF5_DIR=/usr/lib/x86_64-linux-gnu/hdf5/openmpi pip install . --user
# 
# git clone https://gitlab.kitware.com/paraview/catalyst.git
#
# mkdir catalyst-build
# cd catalyst-build
# ccmake -G Ninja [path to catalyst source directory]
#
# # do the build
# ninja
# # do the install
# sudo ninja install
# 
# # Now make a simulation that uses conduit
# 
# cmake -Dcatalyst_DIR=/usr/local/lib/cmake/catalyst-2.0  sim-dir
#

#
# ## These are for Ascent and Conduit
# export PYTHONPATH=/home/jbowden/local/python-modules:$PYTHONPATH
# 
#  mpirun -np 4 --oversubscribe ./test_dask_blocks.py
#
# ## This is for ParaView (not needed if we use pvbatch or pvpython)
# export PYTHONPATH=/home/jbowden/local/lib/python3.8/site-packages:$PYTHONPATH
# This is for using Paraview (not need if using Ascent)
#
# export CATALYST_IMPLEMENTATION_PATHS="<paraview-install-dir>/lib/catalyst" 
# export CATALYST_IMPLEMENTATION_NAME=paraview
# mpirun -np 4 --oversubscribe pvbatch --sym --mpi ./test_dask_blocks.py
# 
# 
from mpi4py import MPI
import dask.array as da
import time
# from dask.distributed import Client
from functools import reduce
from operator import mul
import numpy as np
import random
import sys
import itertools
# import conduit

from operator import  attrgetter

import argparse

#----------------------------------------------------------------
# parse command line arguments
parser = argparse.ArgumentParser(description="Wavelet MiniApp for Catalyst testing")
parser.add_argument("-t", "--timesteps", type=int, help="number of timesteps to run the miniapp for (default: 100)", default=100)
parser.add_argument("--size",  type=int, help="number of samples in each coordinate direction (default: 101)", default=101)
parser.add_argument("-s", "--script", type=str, action="append", help="path(s) to the Catalyst script(s) to use for in situ processing. Can be a "
    ".py file or a Python package zip or directory", required=False)
parser.add_argument("--script-version", type=int,  help="choose Catalyst analysis script version explicitly, otherwise it "
    "will be determined automatically. When specifying multiple scripts, this "
    "setting applies to all scripts.", default=0)
parser.add_argument("-d", "--delay", type=float, help="delay (in seconds) between timesteps (default: 0.0)", default=0.0)
parser.add_argument("-c", "--channel", type=str, help="Catalyst channel name (default: input)", default="input")


 
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
    
    dask_str=''
    for dim in reversed(range(0, tup_len)):
        dask_str+= '[' 
    
    # initialize inputs
    first_elem = False 
    p0 = myblocks_sorted[0].offset_tpl
    # add first tuple to list
    dask_str+=str(p0)
    t1 = 1
    while t1 < len(myblocks_sorted):
        p1 = myblocks_sorted[t1].offset_tpl
        dim = tup_len-1
        sepStr , first_elem =  getSeparator2(p0, p1, dim, '',first_elem) 

        dask_str += sepStr + str(p1)
        t1 = t1 + 1
        p0 = p1
      
    for dim in reversed(range(0, tup_len)):
        dask_str+= ']'
      
    return dask_str
    

class SubBlock:
  def __init__(self, blocknum, dims_tpl, offset_tpl, total_blocks,np_sctn):
    self.blocknum = blocknum
    self.dims_tpl = dims_tpl
    self.offset_tpl = offset_tpl
    self.total_blocks = total_blocks
    self.np_sctn = np_sctn
    #print(self.offset_tpl)
  def __repr__(self):
    return repr((self.offset_tpl))


# Creates a list of ranges that can be iterated through using itertools.product()
# ranges = [
    # range(0, domain_D, bloc_D),
    # range(0, domain_H, bloc_H),
    # range(0, domain_W, bloc_W),
    # range(0, domain_V, bloc_V)
# ]
def makeRanges( domain_tup, bloc_tup ):
    if len(domain_tup) != len(bloc_tup):
        raise Exception("len(domain_tup) != len(bloc_tup)" )
        
    # Should do checks here that domain is perfect multiple of block
    for dim in range(0,len(domain_tup)):
        if domain_tup[dim] % bloc_tup[dim] != 0:
            raise Exception("Dimension dim = " + str(dim) + 
                " is not perfectly divisible. domain_tup[dim] % bloc_tup[dim] = " 
                +str(domain_tup) + " % " 
                +str(bloc_tup) )
            
    ranges = []
    for dim in range(0,len(domain_tup)):
        ranges.append(range(0,domain_tup[dim],bloc_tup[dim]))
        
    return ranges
    



def makeSortedBlocks( ranges, bloc_tup ):
    # Multiply the length of each range to get the total number of iterations
    total_blocs = reduce(mul,([len(v) for v in ranges]))
    blocknum_lst_rnd = random.sample(range(0,total_blocs),total_blocs) # no repeat values
    # print((blocknum_lst_rnd))
    myblocks = []

    # This is equivalent to a nested for loop of the ranges in ranges
    for args in itertools.product(*ranges):
        # print(args)
        blocknum = blocknum_lst_rnd.pop()  # goes backwards through list and removes the last item.
        myblocks.append(SubBlock(blocknum, bloc_tup, args, total_blocs))

    return myblocks, total_blocs
    
    
def makeSortedBlocksWithData( ranges, bloc_tup, split_type, rank, size ):
    
    # Multiply the length of each range to get the total number of iterations
    # e.g. [0,2,4,8],[0,1,2,3,4,5,6,7],[0,1,2]  == 4 x 7 x 3
    total_blocs = reduce(mul,([len(v) for v in ranges]))
    blocknum_lst_rnd = random.sample(range(0,total_blocs),total_blocs) # no repeat values
    # print((blocknum_lst_rnd))
    myblocks = []

    # convert split_type  to integer tuple
    # e.g. split_type (1 ,0, 0, 0)
    bloc_dim_step   = [int(v > 0) * (size) for v in split_type] 
    bloc_dim_lower  = tuple([int(v > 0) * (rank * size) for v in split_type])
    bloc_dim_upper  = tuple(map(lambda i, j: i + j, bloc_dim_lower, bloc_dim_step))
    # print((bloc_dim_step))
    
    pos = [i for i, e in enumerate(bloc_dim_step) if e != 0][0]
    # print(rank, 'bloc_dim_lower: ',bloc_dim_lower)
    # print(rank, 'bloc_dim_upper: ',bloc_dim_upper)
    
    # This is equivalent to a nested for loop of the ranges in the 'ranges' object
    creation = 0
    for args in itertools.product(*ranges):
        # if rank == 1: print(rank, ' args ', args)
        blocknum = blocknum_lst_rnd.pop()  # goes backwards through list and removes the last item.
        # res = tuple(ele1 // ele2 for ele1, ele2 in zip(args, bloc_dim_p1)) 
        
        args_zeroed = list(args)
        for i in range( pos ):
            args_zeroed[i] = 0
        args_zeroed = tuple(args_zeroed)  
        
       
        # if (args_zeroed) >= (bloc_dim_lower) and (args_zeroed)  < (bloc_dim_upper) : 
        #    #print(rank, ' args_zeroed: ',args_zeroed)
        #    print('rank: ', rank, ' making block:        ',blocknum, '  size: ', bloc_tup)
        if creation % size == rank:
            np_sctn = np.ones(bloc_tup) * rank    
        else:
            np_sctn = None
            
        myblocks.append(SubBlock(blocknum, bloc_tup, args, total_blocs, np_sctn))
        creation += 1
        
    return myblocks, total_blocs
    
    
# Sort on the randomized block number to create a unsorted list of offset tuples
# myblocks_unsorted= sorted(myblocks, key=attrgetter('blocknum'), reverse=False)
# The following could be used to re-order the randomized tuple data
# myblocks_sorted= sorted(myblocks_unsorted, key=attrgetter('offset_tpl'), reverse=False)

 
def main(args) :
    try:
        # from paraview.catalyst import bridge
        # from paraview import print_info, print_warning
        # bridge.initialize()
        np.random.seed(seed=42)
        comm = MPI.COMM_WORLD
        rank = comm.Get_rank()
        size = comm.Get_size()
        
        # This code is generic for any rank of
        # arrays to be tested
        # We build the dimensions here.
        # Domain size:
        # domain_V = 16
        domain_W = 16
        domain_H = 16  # block this into 3
        # domain_D = 16 # block this into 4

        # Block sub-domain sizes:
        # bloc_V   = domain_V  // 1
        bloc_W   = domain_W  // 4
        bloc_H   = domain_H  // 4
        # bloc_D   = domain_D  // 4 

        # bloc_tup        = (bloc_D, bloc_H, bloc_W, bloc_V)
        # domain_tup      = (domain_D, domain_H, domain_W, domain_V)        
        # bloc_tup        = (bloc_D, bloc_H, bloc_W)
        # domain_tup      = (domain_D, domain_H, domain_W)
        bloc_tup        = (bloc_H, bloc_W)
        domain_tup      = (domain_H, domain_W)
        
        # split_type indicates which dimension the domain decomposition has been applied over.
        # split_type = ('block', '', '')  # block or spread are the options
        split_type  = tuple(map(lambda i, j: int(i < j) , bloc_tup, domain_tup))
        split_type  = (0,1,0)
        # print(split_type)
        
        ranges          = makeRanges( domain_tup, bloc_tup )
        # myblocks_sorted, total_blocs = makeSortedBlocks( ranges, bloc_tup )
        myblocks_sorted, total_blocs = makeSortedBlocksWithData( ranges, bloc_tup, split_type, rank, size  )
        
        dask_str = returnDaskBlockLayout(myblocks_sorted)
        if rank == 0:
            print(dask_str) 
            
        # exec(dask_str)
        # x = da.block(data)
        # y = x *2
        # print(x.compute())
        # print('')
        # print(y.compute())
        # print('')
        # print(y.chunks)

        
        # Create split numpy arrays distributed over ranks
    finally:
        # finalize Catalyst
        # bridge.finalize()    
        pass
  




# This will be for visualization. Currently not used
def output_via_ascent(np_data, iteration):
    import conduit
    import conduit.blueprint
    import ascent
    import numpy as np

    # print details about ascent
    print(ascent.about())


    # open ascent
    # a = ascent.Ascent()
    # a.open()
    # open ascent
    a = ascent.mpi.Ascent()
    ascent_opts = conduit.Node()
    ascent_opts["mpi_comm"].set(MPI.COMM_WORLD.py2f())
    a.open(ascent_opts)


    # create example mesh using conduit blueprint
    n_mesh = conduit.Node()
    # Explicit examples are available here: https://llnl-conduit.readthedocs.io/en/latest/blueprint_mesh.html#hexs
    # conduit.blueprint.mesh.examples.braid("hexs",  11, 11, 11,  n_mesh)
    conduit.blueprint.mesh.examples.basic("hexs",  11, 11, 11,  n_mesh)
    
    n_mesh["state/domain_id"] = MPI.COMM_WORLD.rank
    # overwrite example field with domain id
    n_mesh["fields/braid/values"][:] = MPI.COMM_WORLD.rank
    
    
    # publish mesh to ascent
    a.publish(n_mesh)

    # declare a scene to render the dataset
    scenes  = conduit.Node()

    if False:
        scenes["s1/plots/p1/type"] = "pseudocolor"
        scenes["s1/plots/p1/field"] = "braid"
    else:
        scenes["s1/plots/p1/type"] = "mesh"
    # scenes["s1/plots/p1/min_value"] = -0.5
    # scenes["s1/plots/p1/max_value"] = 0.5
    scenes["s1/image_prefix"] = "out_ascent_render_3d_"

    scenes["s1/renders/r1/image_width"]  = 512
    scenes["s1/renders/r1/image_height"] = 512
    # Set the output file name (ascent will add ".png")
    scenes["s1/renders/r1/image_prefix"] = "out_ascent_render2_3d_"



    vec3 = np.array([1.0,1.0,1.0], dtype=np.float32)
    vec3[0] = 1.
    vec3[1] = 1.
    vec3[2] = 1.
    scenes["s1/renders/r1/camera/look_at"].set(vec3);  # deep copied

    # position interacts with near_plane and far_plane by clipping
    # data outside the range
    vec3[0] = 20
    vec3[1] = 20.
    vec3[2] = 20.
    scenes["s1/renders/r1/camera/position"].set(vec3);
    vec3[0] = 0.
    vec3[1] = 0.
    vec3[2] = 1.
    scenes["s1/renders/r1/camera/up"].set(vec3);
    scenes["s1/renders/r1/camera/fov"] = 80.;
    scenes["s1/renders/r1/camera/xpan"] = 0.0;
    scenes["s1/renders/r1/camera/ypan"] = 0.0;
    scenes["s1/renders/r1/camera/azimuth"] = 25.0;
    scenes["s1/renders/r1/camera/elevation"] = -10.0;
    scenes["s1/renders/r1/camera/zoom"] = 0.5;
    scenes["s1/renders/r1/camera/near_plane"] = 0.1;
    scenes["s1/renders/r1/camera/far_plane"] = 100.1;



    # setup actions to
    actions = conduit.Node()
    add_act =actions.append()
    add_act["action"] = "add_scenes"
    add_act["scenes"] = scenes

    # execute
    a.execute(actions)

    # close alpine
    a.close()



 
if __name__ == "__main__":
    args = parser.parse_args()
    main(args)   
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
