import dask.array as da
import numpy as np
from dask.distributed import Client


class DaskStats:
    import dask.array as da
    import numpy as np
    """
     A class used to hold a Dask array version of streaming statistics.
     
     This class can return the average and standard deviation of data that
     has been pushed to it via the update() method. The pushed data must be
     the same shape as the all prior data that has been pushed. The intermediate results 
     are saved to the Dask scheduler and can be updated by multiple Dask clients. This requires
     a unique and known name to be used
     
    The method is based on Welford's online algorithm. See:
    (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#:~:text=to%20a%20degree.-,Welford%27s%20online%20algorithm,-%5Bedit%5D)

     
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
        self.mean  = da.full_like(array_of_shape, 0.0)
        self.M2    = da.full_like(array_of_shape, 0.0)
        
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
    
    
