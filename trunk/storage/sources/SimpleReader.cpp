/* 
 * File:   SimpleReader.cpp
 * Author: cata
 * 
 * Created on May 7, 2013, 1:33 AM
 */
#include "storage/SimpleReader.hpp"
#include "core/Debug.hpp"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"


namespace Damaris{   
    
    SimpleReader::SimpleReader(Variable* v,std::string magicNumber){
        this->var = v;
        this->lastIteration = 0;
        initFile(magicNumber);
    }   

    SimpleReader::~SimpleReader(){
      MPI_File_close(&damarisFile);
    }

    std::map<int,DataSpace*> SimpleReader::Read(int iteration){
        
        void *data;
        MPI_Status status;   
      
        std::map<int,DataSpace*> dataSpaceArray;
        ChunkInfo* readChunk = (ChunkInfo*)calloc(1,sizeof(ChunkInfo));    
        int error;
        MPI_Offset off=0,offBack=0;
        MPI_Offset sizeFile;
        
        if (iteration < lastIteration) 
            readChunk = jumpBackwards(iteration); 
        if (iteration > lastIteration)
            readChunk = jumpForward(iteration);
        if (iteration == lastIteration){
            error=MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo), MPI_BYTE, &status );          
            if (error != MPI_SUCCESS){  
                 WARN("EMPTY");
                 return dataSpaceArray;
            }
          
           lastIteration++;   
            
        }
        if (readChunk == NULL){
            WARN("readChunk is null");
            return dataSpaceArray;
        }
        
      
        MPI_File_get_size(damarisFile,&sizeFile);
        MPI_File_get_position(damarisFile,&off);
       
        //reading multiple chunks for the same iteration
        while(readChunk->iteration == iteration && off<sizeFile){
            
                    
            data = malloc(readChunk->size);             
            MPI_File_read(damarisFile,data,readChunk->size,MPI_BYTE,&status);               
            dataSpaceArray[readChunk->pid] = new DataSpace(data, readChunk->size);
            MPI_File_get_position(damarisFile,&offBack);
            MPI_File_read(damarisFile,readChunk,sizeof(ChunkInfo),MPI_BYTE,&status);
            MPI_File_get_position(damarisFile,&off);     
          
            free(data); 
            
           
        }
        MPI_File_seek(damarisFile,offBack,MPI_SEEK_SET);        
        //it doesn't work
        //MPI_File_seek(damarisFile,-1*sizeof(ChunkInfo),MPI_SEEK_CUR);            
        return dataSpaceArray;                  
      

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
