import dask.array as da
import numpy as np
from dask.distributed import Client, Lock


class DaskStats:
    import dask.array as da
    import numpy as np
    """
     A class used to hold a Dask array version of streaming statistics.
     
     This class will return the average and standard deviation of data that
     has been pushed to it via the update() method. The pushed data must be
     the same shape as the Dask array that initialised the class. The intermediate results 
     are saved to the Dask scheduler and can be updated by multiple Dask clients. This requires
     a unique and known name to be used in save_on_dask() and retrieve_from_dask() methods.
     
    The method is based on Welford's online algorithm. See:
    (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#:~:text=to%20a%20degree.-,Welford%27s%20online%20algorithm,-%5Bedit%5D)

     
     Methods 
     -------
     
     
    """
    def __init__(self,  unique_name_str, lock_name_str ):
        """
        Constructor
        
        unique_name_str : Is the name given to the average, M2 and count datasets when saved to 
                          the Client.datasets[] dictionary. 
                          Client.datasets[unique_name_str+'_count']
                          Client.datasets[unique_name_str+'_mean']
                          Client.datasets[unique_name_str+'_M2']
                          The dictionary values are read only, so 
                          we have to remove them before updating them with new values
                          
        lock_name_str: Is a name to use for the lock, so other Dask clients can ask for it by name.
        
        dasklock : is a Dask lock, used so that only one client can update data at a time.
        """

        self.lock_name_str   = lock_name_str
        # self.dasklock        = Lock(name=lock_name_str)
        self.unique_name_str = unique_name_str
        # self.chunks_tup      = None  # set on the first call to update()
        
            
        
        
    def update(self, newValue, client, lock_timeout=60):
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
        dasklock        = Lock(name=self.lock_name_str)
        if dasklock.acquire(timeout=lock_timeout):
            if self.unique_name_str+'_count' in client.list_datasets():
                count      = client.datasets[self.unique_name_str+'_count']
                chunks_tup = client.datasets[self.unique_name_str+'_chunks_tup']
                mean       = client.datasets[self.unique_name_str+'_mean']
                M2         = client.datasets[self.unique_name_str+'_M2']
            else: # initialise
                count      = 0    
                chunks_tup = None 
                mean       = da.zeros_like(newValue, dtype=np.float64)
                M2         = da.zeros_like(newValue, dtype=np.float64)
            
            assert newValue.shape == mean.shape    
            
            if chunks_tup == None:
                chunks_tup = self.set_chunks(newValue.shape)
            
            count += 1
            delta = newValue - mean
            # delta.persist()
            mean += delta / count
            # mean.persist()
            delta2 = newValue - mean
            M2 += delta * delta2
            # M2.persist()
            
            # this will unpublish_dataset() first
            self.save_on_dask(client, count, chunks_tup, mean,  M2 )
            
            dasklock.release()
        
        else:
            dasklock.release()
            raise TimeoutError('damaris_stats: Dask lock named: ' + self.lock_name_str + ' timed out')
            

        
        
    def save_on_dask(self, client, count, chunks_tup, mean,  M2 ):
        """
        save_on_dask
        
        client:  A Dask distributed client
        count:   Integer, the number of times data have been updated
        mean:    A NumPy array containing the current mean of data at each element of the array
        M2:      A Numpy array containing the M squared values for computing streaming standard deviations
        
        Saves our data to the Dask server so we can retirve it from another client
        
        """
        if self.unique_name_str+'_count' in client.list_datasets():
            client.unpublish_dataset(self.unique_name_str+'_count') 
            client.unpublish_dataset(self.unique_name_str+'_chunks_tup')             
            client.unpublish_dataset(self.unique_name_str+'_mean')  
            client.unpublish_dataset(self.unique_name_str+'_M2')   
            
        client.datasets[self.unique_name_str+'_count'] = count   
        client.datasets[self.unique_name_str+'_chunks_tup'] = chunks_tup        
        client.datasets[self.unique_name_str+'_mean']  = mean  
        client.datasets[self.unique_name_str+'_M2']    = M2  
    
    def return_count(self, client, lock_timeout=60):
        """
        return_count
        
        Retrieve the Dask 'count' 
        """
        dasklock        = Lock(name=self.lock_name_str)
        if dasklock.acquire(timeout=lock_timeout):
            if self.unique_name_str+'_count' in client.list_datasets():
                count = client.datasets[self.unique_name_str+'_count']
            else: # initialise
                count     = 0    
                
            dasklock.release()    
                
            return count
        else:
            dasklock.release()
            raise TimeoutError('Dask lock: ' + self.lock_name_str + ' timed out') 
            
    def return_mean(self, client, lock_timeout=60):
        """
        return_mean
        
        Retrieve the Dask array 'future' self.mean
        """
        dasklock        = Lock(name=self.lock_name_str)
        if dasklock.acquire(timeout=lock_timeout):
            if self.unique_name_str+'_count' in client.list_datasets():
                count = client.datasets[self.unique_name_str+'_count']
                mean  = client.datasets[self.unique_name_str+'_mean']
            else: # initialise
                count     = 0    
                mean      = da.zeros_like(newValue, dtype=np.float64)
                
            dasklock.release()    
                
            if count < 2:
                return da.full_like(mean, float("nan")) 
            else:
                return mean.persist()
        else:
            dasklock.release()
            raise TimeoutError('Dask lock: ' + self.lock_name_str + ' timesd out')    
            
            
    def return_variance(self, client, lock_timeout=60):
        """
        return_variance
        
        Calculate and return the Dask array sample variance 
        
        Computed as: self.M2 / (self.count - 1)
        
        Returns a 'future', so to get the values we must .compute() or .persist()
        """
        dasklock        = Lock(name=self.lock_name_str)
        if dasklock.acquire(timeout=lock_timeout):
            if self.unique_name_str+'_count' in client.list_datasets():
                count = client.datasets[self.unique_name_str+'_count']
                M2    = client.datasets[self.unique_name_str+'_M2']
            else: # initialise
                count     = 0    
                mean      = da.zeros_like(newValue, dtype=np.float64)
                M2        = da.zeros_like(newValue, dtype=np.float64)

        else:
            dasklock.release()
            raise TimeoutError('Dask lock: ' + self.lock_name_str + ' timed out')    
        
        dasklock.release()  
        
        if count < 2:
            return da.full_like(mean, float("nan")) 
        else:
            sampleVariance = ( M2 / (count - 1))
        return sampleVariance.persist()  
          
         
         
    def return_mean_var_tuple(self, client, lock_timeout=60):
        return (self.return_mean(client, lock_timeout=lock_timeout), self.return_variance(client, lock_timeout=lock_timeout))    


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
        
        This is a data-reduction type operation. It reduces a Dask array to a numpy array 
        with the same number of elements as the dask array's chuncks
        
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
        
        This is a data-reduction type operation. It reduces a Dask array to a numpy array 
        with the same number of elements as the dask array's chuncks
        
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


        
    def set_chunks(self,   shape  ):
        """
        set_chunks
        
        shape : is a tuple of dimensions of the array  (i.e. from np.shape(num_array)) 
        
        Returns a tuple of 1's the length of the number of dimensions of the input array.
        
        e.g. if array has 3 dims then returns (1, 1, 1). 
        
        This is useful for the chunks= attribute of map_blocks()
        
        """   
        tuplist = list()
        for i in range(len(shape)):
            tuplist.append(1)
        chunks_tup=tuple(tuplist)
        return chunks_tup 
    
    def get_chunks( self, client  ):
        """
        get_chunks
        
        client : A Dask Client object that has access to the scheduler that we saved the tuple to in the update() method
        
        Returns a tuple of 1's the length of the number of dimensions of the input array.
        
        e.g. if array has 3 dims then returns (1, 1, 1). 
        
        This is useful for the chunks= attribute of map_blocks()
        
        """   
        return client.datasets[self.unique_name_str+'_chunks_tup']
