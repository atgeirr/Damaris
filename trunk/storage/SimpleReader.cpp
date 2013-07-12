/* 
 * File:   SimpleReader.cpp
 * Author: cata
 * 
 * Created on May 7, 2013, 1:33 AM
 */
#include "SimpleReader.hpp"
#include "core/Debug.hpp"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"


namespace Damaris{   
    
    
    SimpleReader::SimpleReader(Variable* v,std::string magicNumber) {
        
        this->var = v;
        this->lastIteration = 0;
        std::string path = getPath(magicNumber);
        std::ostringstream processID;   
        processID<<Process::Get()->getID();    
        std::string fileName = path + "/" + processID.str();    
        int error=MPI_File_open(MPI_COMM_SELF, fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);
        
        if (error != MPI_SUCCESS){
            
            ERROR("Error opening file");
            exit(0);
        }
    }

    SimpleReader::SimpleReader(const SimpleReader& orig) {
    }

    SimpleReader::~SimpleReader(){
      MPI_File_close(&damarisFile);
    }

    std::vector<DataSpace*> SimpleReader::Read(int iteration){
        
        void *data;
        MPI_Status status;   
        //Damaris::DataSpace* dataSpace;
        std::vector<DataSpace*> dataSpaceArray;
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
            dataSpaceArray.push_back(new DataSpace(data, readChunk->size));   
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
    


std::string SimpleReader::getPath(std::string magicNumber) {  
    std::string path = StorageManager::basename + "/"+ magicNumber + "/" + this->var->GetName();    
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