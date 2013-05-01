#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include "core/ParameterManager.hpp"
#include "hdf5.h"
#include <hdf5_hl.h>

#define FILE "ex.h5"
#define RANK 2
#define DIM0 100

extern "C" {

void my_function(const std::string& event, int step, int32_t src, const char* args)
{
	        

        hid_t file_id,dataset_id,dataspace_id,dataset_id2;
        herr_t status;
        hsize_t dims[2];
        
        int i,j,dset_data[4][6],dset_read[4][6];
        
        Damaris::Parameter *a = Damaris::ParameterManager::Search("w");
        
        for (i=0;i<4;i++)
            for(j=0;j<6;j++)
                dset_data[i][j]=i*6 + j + 1;
  
           
         
        file_id = H5Fcreate(FILE,H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        dims[0] = 4;
        dims[1] = 6; 
       
        //Create dataspace and dataset
        dataspace_id = H5Screate_simple(2,dims,NULL);
       
        dataset_id = H5Dcreate(file_id,"/dset",H5T_STD_I32BE,dataspace_id,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
       
        
        //write into a dataset
        status = H5Dwrite(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,dset_data);    
        
        //read from a dataset
        status = H5Dread(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,dset_read);
        
        status = H5Dclose(dataset_id);
       
        status = H5Sclose(dataspace_id);
        status = H5Fclose(file_id); 
        
        /*for (i=0;i<4;i++){
            std::cout<<"\n";
            for(j=0;j<6;j++)
                std::cout<<dset_read[i][j]<<" ";
        }*/
         std::cout<<a->GetType()<<"\n";     
        /*if(a!=NULL)
            std::cout<<"val "<<a->GetValue<int>()<<"\n";
        else
            std::cout<<"--- hello world from Damaris ---\n";*/
        
       
        
}

}
