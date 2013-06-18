/* 
 * File:   SimpleReader.cpp
 * Author: cata
 * 
 * Created on May 7, 2013, 1:33 AM
 */

#include "core/Debug.hpp"
#include "SimpleReader.h"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"


namespace Damaris{
    
    
    
    SimpleReader::SimpleReader(Variable* v) {
        
        this->var = v;
        this->lastIteration = 0;
        std::string path = getPath();
        std::ostringstream processID;   
        processID<<Process::Get()->getID();    
        std::string fileName = path + "/" + processID.str();    
        int error=MPI_File_open(MPI_COMM_SELF, fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);
       
        if (error != MPI_SUCCESS)
            ERROR("Error opening file");
    }

    SimpleReader::SimpleReader(const SimpleReader& orig) {
    }

    SimpleReader::~SimpleReader() {
      MPI_File_close(&damarisFile);
    }

    DataSpace* SimpleReader::Read(int iteration){
        
        void *data;
        MPI_Status status;   
        Damaris::DataSpace* dataSpace;
        ChunkInfo* readChunk = (ChunkInfo*)calloc(1,sizeof(ChunkInfo));    
        int error;
        
        if (iteration < lastIteration) 
            readChunk = jumpBackwards(iteration); 
        if (iteration > lastIteration)
            readChunk = jumpForward(iteration);
        if (iteration == lastIteration){
            error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );          
            if (error != MPI_SUCCESS)               
                 return NULL;
             lastIteration++;   
            
        }
        if (readChunk == NULL){
            WARN("readChunk is null");
            return NULL;
        }
        
        data = malloc(readChunk->size);
        MPI_File_read(damarisFile,data,readChunk->size,MPI_BYTE,&status);                     
        //chunk->SetIteration(iteration);
        dataSpace = new DataSpace(data,readChunk->size);   
        size_t size = dataSpace->MemCopy(data);
        if(size == readChunk->size)
            return dataSpace;
        return NULL;
                
        //std::cout<<(char*)data<<std::endl;
        //chunk->SetDataSpace(dataSpace);
        //this->var->AttachChunk(chunk);       
        //return true;

    }
    


std::string SimpleReader::getPath() {  
    std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
    return path;
}

ChunkInfo* SimpleReader::jumpBackwards(int iteration){
    
    ChunkInfo* readChunk = (ChunkInfo*) calloc(1,sizeof(ChunkInfo));       
    MPI_Status status; 
    MPI_Offset currentOffset=0;
    int error;
    
    MPI_File_seek(damarisFile, 0, MPI_SEEK_SET );
    error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );
    if (error != MPI_SUCCESS)               
                 return NULL;
    //TODO: verify this and the offset
    while(readChunk->iteration != iteration){
         currentOffset=(MPI_Offset)readChunk->size;
         MPI_File_seek(damarisFile,currentOffset,MPI_SEEK_CUR);
         error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );
         if (error != MPI_SUCCESS)               
                 return NULL;
    }
    
    return readChunk;
}

ChunkInfo* SimpleReader::jumpForward(int iteration){
    
    ChunkInfo* readChunk =(ChunkInfo*) calloc(1,sizeof(ChunkInfo));       
    MPI_Status status; 
    MPI_Offset currentOffset=0;
    int error ;
    
    if (Environment::GetLastIteration()<iteration){
        WARN("You are trying to read an iteration that wasn't written so far");
        return NULL;
    }
    error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );
    if (error!= MPI_SUCCESS)               
        return NULL;
   
    
    while(readChunk->iteration != iteration){
         currentOffset=readChunk->size;         
         MPI_File_seek(damarisFile,currentOffset,MPI_SEEK_CUR);
         error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );        
         if (error != MPI_SUCCESS)               
                 return NULL;
    }
    
    return readChunk;
}

} 