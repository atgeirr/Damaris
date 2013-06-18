#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include "core/ParameterManager.hpp"
#include "core/VariableManager.hpp"
#include "core/Environment.hpp"
#include "data/Chunk.hpp"
#include "xml/Model.hpp"
//#include "storage/StorageManager.h"

//#include "hdf5.h"
//#include <hdf5_hl.h>

#define FILE "ex.h5"
#define RANK 3
#define DIM0 100

extern "C" {

void hdf5_write(const std::string& event, int step, int32_t src, const char* args)
{
	        

        /*hid_t file_id,dset_id,dpace_id,cparms;
        herr_t status;
        hsize_t dims[RANK]={3,3,3};
        hsize_t chunk_dims[RANK]={2,2,5};
        hsize_t maxdims[RANK]={H5S_UNLIMITED,H5S_UNLIMITED};
        
        int i,dset_data[4][6],dset_read[4][6];
        unsigned int j;
        
        //create the hdf5 file
       
        
        file_id = H5Fcreate(FILE,H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        dspace_id = H5Screate_simple(RANK,dims,maxdims);
        cparms = H5Pcreate(H5P_DATASET_CREATE);
        status = H5Pset_chunk(cparms, RANK, chunk_dims);
        
        Damaris::VariableManager::iterator it = Damaris::VariableManager::Begin();
       
        for (; it!=Damaris::VariableManager::End();it++){
            
            Damaris::Variable *var = it->get();    
            Damaris::Chunk* chunk = var->GetChunk(src,step);
            unsigned int dimensions = chunk->GetDimensions();
            
                 
            int* data= (int*)chunk->Data();
            for( j=0;j<dimensions;j++){
                for (i=chunk->GetStartIndex(j);i<chunk->GetEndIndex(j);i++){
                        std::cout<<*(data + i)<<" ";
                    
                }
            }
            std::cout<<std::endl;
            
           
        }
             
        //Create dataspace and dataset
        //dataspace_id = H5Screate_simple(2,dims,NULL);
       
        //dataset_id = H5Dcreate(file_id,"/dset",H5T_STD_I32BE,dataspace_id,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
       
        
        //write into a dataset
        //status = H5Dwrite(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,dset_data);    
        
        //read from a dataset
        //status = H5Dread(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,dset_read);
        
        //status = H5Dclose(dataset_id);
       
        //status = H5Sclose(dataspace_id);
          status = H5Fclose(file_id);       
        //std::list<int> ids = Damaris::Environment::GetKnownLocalClients();*/  
    
        
        
  
        
       
        
}

}
