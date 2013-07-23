/* 
 * File:   DifferentialReader.cpp
 * Author: cata
 * 
 * Created on July 5, 2013, 5:36 PM
 */

#include "DifferentialReader.hpp"
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
            int error=0;
            std::string path = getPath(magicNumber);
            std::ostringstream processID;   
            processID<<Process::Get()->getID();    
            std::string fileName = path + "/" + processID.str();   
       
            error=MPI_File_open(MPI_COMM_SELF, (char*)fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);

            if (error != MPI_SUCCESS){
                ERROR("Error opening file");               
                exit(0);
            }
    }

    DifferentialReader::DifferentialReader(const DifferentialReader& orig) {
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
        //std::vector<DataSpace*> dataSpaceArray;
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
                //char* lastData = (char*) last->data;
                newData = (Bytef*) malloc(readChunk->uncompressedSize);                
                for (int i=0; i<readChunk->uncompressedSize;i++)
                    newData[i] = uncompressedData[i] ^ last->data[i];            
                 
                replace->data = newData;   
                std::cout<<"Inserting at"<<readChunk->pid<<std::endl;
                //dataSpaceArray.push_back(new DataSpace(newData, readChunk->uncompressedSize));   
                //dataSpaceArray.insert(dataSpaceArray.begin() + readChunk->pid,new DataSpace(newData, readChunk->uncompressedSize));
                dataSpaceArray[readChunk->pid] = new DataSpace(newData, readChunk->uncompressedSize);
            }
            else{                
                //dataSpaceArray.push_back(new DataSpace(compressedData, readChunk->uncompressedSize));
                std::cout<<"Inserting at"<<readChunk->pid<<std::endl;
                //std::cout<<"Vector size"<<dataSpaceArray.size()<<std::endl;
                //dataSpaceArray.insert(dataSpaceArray.begin() + readChunk->pid,new DataSpace(compressedData, readChunk->uncompressedSize));
                dataSpaceArray[readChunk->pid] = new DataSpace(compressedData, readChunk->uncompressedSize);
                std::cout<<"Reading at iteration 0: "<<(int)compressedData[0]<<" "<<readChunk->pid<<std::endl;
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

    std::string DifferentialReader::getPath(std::string magicNumber) {  
        std::string path = StorageManager::basename + "/"+ magicNumber + "/" + this->var->GetName();    
        return path;
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
         std::cout<<"return null"<<std::endl;
         return NULL;
    }
    
    
}

