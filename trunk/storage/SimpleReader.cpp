/* 
 * File:   SimpleReader.cpp
 * Author: cata
 * 
 * Created on May 7, 2013, 1:33 AM
 */

#include "SimpleReader.h"
#include "data/Chunk.hpp"
#include "mpi.h"
#include "StorageManager.h"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include <boost/filesystem.hpp>

namespace Damaris{
    
    SimpleReader::SimpleReader(Variable* v) {
        this->var = v;
    }

    SimpleReader::SimpleReader(const SimpleReader& orig) {
    }

    SimpleReader::~SimpleReader() {
    }

    bool SimpleReader::Read(int iteration, Chunk *chunk){
        
        int ok;               
        MPI_Offset fileSize,currentOffset;
        MPI_File damarisFile;
        void *data;
        MPI_Status status;   
        Damaris::DataSpace* dataSpace;
        ChunkInfo readChunk;  
     

        ok = 1;
        currentOffset = 0;
        
        /*Open file*/
        std::string path = getPath();
        std::ostringstream processID;   
        processID<<Process::Get()->getID();
    
        boost::filesystem::create_directories(boost::filesystem::path(path.c_str()));  
        std::string fileName = path + "/" + processID.str();
    
        MPI_File_open(MPI_COMM_SELF, fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);
       
        //set the file cursor at 0
        MPI_File_seek( damarisFile, 0, MPI_SEEK_SET );
        //get the file size
        MPI_File_get_size(damarisFile, &fileSize);
        
         
        while (currentOffset<fileSize && ok == 1){
            
           MPI_File_read(damarisFile,&readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );
           
           if(readChunk.iteration == iteration){
               
               data = malloc(readChunk.size);
               MPI_File_read(damarisFile,data,readChunk.size,MPI_BYTE,&status);
               ok =0;
               
               //set chunk               
               chunk->SetIteration(iteration);
               dataSpace = new DataSpace(data,readChunk.size);              
               chunk->SetDataSpace(dataSpace);
               
               if (status.MPI_ERROR != MPI_SUCCESS) {                   
                 MPI_File_close(&damarisFile);
                 return false;
               }
           }
           else{
               currentOffset+=(MPI_Offset)readChunk.size;
               MPI_File_seek(damarisFile,currentOffset,MPI_SEEK_SET);
           }
            
        }
        
        this->var->AttachChunk(chunk);
  
        MPI_File_close(&damarisFile);     
        
        return true;

    }
    


std::string SimpleReader::getPath() {  
        std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
        return path;
}
}