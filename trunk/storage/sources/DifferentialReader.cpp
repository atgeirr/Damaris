/* 
 * File:   DifferentialReader.cpp
 * Author: cata
 * 
 * Created on July 5, 2013, 5:36 PM
 */

#include "storage/DifferentialReader.hpp"
#include "core/Debug.hpp"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"
#include "zlib.h"


namespace Damaris{


    DifferentialReader::DifferentialReader(Variable* v,std::string magicNumber) {
        this->var = v;
        this->lastIteration = 0;
        initFile(magicNumber);
    }
    DifferentialReader::~DifferentialReader() {
          MPI_File_close(&damarisFile);
          //TODO: free memory
    }
    
    std::map<int,DataSpace*> DifferentialReader::Read(int iteration){
               
        Bytef* uncompressedData,*newData;
        Bytef* compressedData;
        LastReadChunk* last;
        MPI_Status status;  
        int error;
        
        std::map<int,DataSpace*> dataSpaceArray;
        DifferentialChunk* readChunk = (DifferentialChunk*)calloc(1,sizeof(DifferentialChunk));    
        MPI_Offset off=0,offBack=0;
        MPI_Offset sizeFile;
        
        if (iteration < lastIteration) 
            readChunk = jumpBackwards(iteration); 
        if (iteration > lastIteration)
            readChunk = jumpForward(iteration);
        if (iteration == lastIteration){           
            error=MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk), MPI_BYTE, &status ); 
           
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
                         
            uncompressedData = (Bytef*)malloc(readChunk->uncompressedSize);    
            compressedData = (Bytef*) malloc(readChunk->size);
            MPI_File_read(damarisFile,compressedData,readChunk->size,MPI_BYTE,&status);
            LastReadChunk* replace = (LastReadChunk*)malloc (sizeof(LastReadChunk));
            replace->size = readChunk->uncompressedSize;
            replace->pid = readChunk->pid;  
            
            if (iteration!=0){
                uLongf destLen = (uLongf) readChunk->uncompressedSize;               
                error = uncompress(uncompressedData,&destLen,compressedData,(uLong)readChunk->size);
                last = getLastBlock(readChunk->pid);                
                newData = (Bytef*) malloc(readChunk->uncompressedSize);
                
                for (int i=0; i<readChunk->uncompressedSize;i++)
                    newData[i] = uncompressedData[i] ^ last->data[i];            
                 
                replace->data = newData;                
                dataSpaceArray[readChunk->pid] = new DataSpace(newData, readChunk->uncompressedSize);
            }
            else{             
                dataSpaceArray[readChunk->pid] = new DataSpace(compressedData, readChunk->uncompressedSize);
                replace->data = compressedData;                
            } 
                    
            partialLastChunks.push_back(replace);         
            
            MPI_File_get_position(damarisFile,&offBack);
            MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk),MPI_BYTE,&status);
            MPI_File_get_position(damarisFile,&off);     
                  
           
            //free(uncompressedData); 
            //free(compressedData);
          
           
        }
        
        lastChunks.clear();        
        lastChunks = partialLastChunks;    
        partialLastChunks.clear();
        
        MPI_File_seek(damarisFile,offBack,MPI_SEEK_SET); 
       
        return dataSpaceArray;   
    }

   

    DifferentialChunk* DifferentialReader::jumpBackwards(int iteration){

        DifferentialChunk* readChunk = (DifferentialChunk*) calloc(1,sizeof(DifferentialChunk));       
        MPI_Status status; 
        MPI_Offset currentOffset=0;
        int error;

        MPI_File_seek(damarisFile, 0, MPI_SEEK_SET );
        error=MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk), MPI_BYTE, &status );
        if (error != MPI_SUCCESS)               
            return NULL;

        while(readChunk->iteration != iteration){
             currentOffset=(MPI_Offset)readChunk->size;
             MPI_File_seek(damarisFile,currentOffset,MPI_SEEK_CUR);
             error=MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk), MPI_BYTE, &status );
             if (error != MPI_SUCCESS)               
                return NULL;
        }

        return readChunk;
    }

    DifferentialChunk* DifferentialReader::jumpForward(int iteration){

        DifferentialChunk* readChunk =(DifferentialChunk*) calloc(1,sizeof(DifferentialChunk));       
        MPI_Status status; 
        MPI_Offset currentOffset=0;
        int error ;

        if (Environment::GetLastIteration()<iteration){
            WARN("You are trying to read an iteration that wasn't written so far");
            return NULL;
        }
        error=MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk), MPI_BYTE, &status );
        if (error!= MPI_SUCCESS)               
            return NULL;   

        while(readChunk->iteration != iteration){
             currentOffset=readChunk->size;         
             MPI_File_seek(damarisFile,currentOffset,MPI_SEEK_CUR);
             error=MPI_File_read(damarisFile,readChunk,sizeof(DifferentialChunk), MPI_BYTE, &status );        
             if (error != MPI_SUCCESS)               
                return NULL;
        }

        return readChunk;
    }
    
    LastReadChunk* DifferentialReader::getLastBlock (int pid){
         
         for(std::vector<LastReadChunk*>::size_type i = 0; i <lastChunks.size(); i++){
            
             LastReadChunk *lastChunk = lastChunks[i];            
             if(lastChunk->pid == pid)
                 return lastChunk;
         }       
         return NULL;
    }
    
    
}

