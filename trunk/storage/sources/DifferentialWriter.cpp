/* 
 * File:   DifferentialWriter.cpp
 * Author: Catalina
 * 
 * Created on July 5, 2013, 5:36 PM
 */

#include "storage/DifferentialWriter.hpp"
#include "zlib.h"
#include "data/Variable.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include <boost/filesystem.hpp>

#define COMPRESS_LEVEL 4

namespace Damaris{
    DifferentialWriter::DifferentialWriter(Variable* v){
        
        this->var = v;
        this->lastIteration = 0;  
        initFile();       
    }

    DifferentialWriter::DifferentialWriter(const DifferentialWriter& orig) {
    }

    DifferentialWriter::~DifferentialWriter() {
         MPI_File_close(&damarisFile);
         //TODO: free memory;
    }
    
    bool DifferentialWriter::Write(int iteration){
        return true; 
    }
    
    bool DifferentialWriter::Write(){
        
        MPI_Status status;
        ChunkIndexByIteration::iterator begin,end, it;    
        Chunk *chunk ;
        int error;
              
        if (Environment::GetLastIteration()<lastIteration){        
            WARN("You are trying to write the same iteration twice");
            return false;
        } 
        
        this->var->GetChunksByIteration(lastIteration,begin,end);
        
        Model::Type t = this->var->GetLayout()->GetType();      
        int typeSize = Types::basicTypeSize(t);
       
        
        for (it=begin; it != end; it++){
            
            DifferentialChunk chunkInfo;
            chunk = it->get();                 
            int pid = chunk->GetSource();
            int bid = chunk->GetBlock();
            int iteration = chunk->GetIteration();
            //int dimensions = chunk->NbrOfItems();                
            unsigned int uncompressedSize = chunk->GetDataSpace()->Size();
                                  
            if (lastIteration != 0) 
                writeXorData (chunk, chunkInfo, uncompressedSize,typeSize);           
            else{                
                createChunkStructure(chunkInfo,pid,iteration,uncompressedSize,bid,uncompressedSize,typeSize);     
                error=MPI_File_write(damarisFile, &chunkInfo, sizeof (DifferentialChunk), MPI_BYTE, &status);
                void* data = chunk->GetDataSpace()->Data();                
                error=MPI_File_write(damarisFile, data, uncompressedSize, MPI_BYTE, &status); 
            }         
            
        }
        
        lastChunks.clear();
       
       for (it = begin; it!=end;it++){
           
            chunk = it->get();
            LastWrittenChunk* last = (LastWrittenChunk*)malloc (sizeof(LastWrittenChunk));
            last->size = chunk->GetDataSpace()->Size();
            Bytef* dataa = (Bytef*)chunk->GetDataSpace()->Data();
           
            last->data =(Bytef*) malloc(chunk->GetDataSpace()->Size()*typeSize);
            for (unsigned int i=0;i<chunk->GetDataSpace()->Size();i++)
               last->data[i] = *(dataa+i);
             
            last->pid = chunk->GetSource();
            lastChunks.push_back(last);
       } 
       lastIteration++;        
       
       return true;
    }
    
    void DifferentialWriter::createChunkStructure(DifferentialChunk &chunkInfo,int pid,int iteration,int size, int bid, int uncompressedSize, int typeSize){
        chunkInfo.pid = pid;
        chunkInfo.iteration = iteration;
        chunkInfo.size = size;
        chunkInfo.bid = bid;
        chunkInfo.typeSize=typeSize;
        chunkInfo.uncompressedSize = uncompressedSize;
    }
    
      
    bool DifferentialWriter::writeXorData (Chunk* chunk, DifferentialChunk chunkInfo, unsigned int uncompressedSize, int typeSize){
        
        MPI_Status status;        
        Bytef* data = (Bytef*) chunk->GetDataSpace()->Data();      
        
        int pid = chunk->GetSource();
        int bid = chunk->GetBlock(); 
        int iteration = chunk->GetIteration(); 
       
        uLongf compressedSize = compressBound(uncompressedSize);
        int size = chunk->GetDataSpace()->Size();
        
        LastWrittenChunk* last = getLastBlock(pid);       
        char* data1 = (char*)last->data;       
        char* newData = (char*)malloc(size);
       
        for (int i=0; i<size;i++)
           newData[i] = data[i] ^ data1[i];           
        
        
        Bytef* dest = (Bytef*)malloc(compressedSize*sizeof(Bytef));
        int error = compress2(dest, &compressedSize, (Bytef *) newData, size, COMPRESS_LEVEL);
               
        if(error!= Z_OK){
                ERROR ("Error on compression");
                return false;
        }
              
        createChunkStructure(chunkInfo,pid,iteration,(int)compressedSize,bid,uncompressedSize,typeSize);   
      
        error=MPI_File_write(damarisFile, &chunkInfo, sizeof (DifferentialChunk), MPI_BYTE, &status);
           
        if (error != MPI_SUCCESS ){   
                ERROR("Error writing to file");        
                return false;
        }
        error=MPI_File_write(damarisFile, dest, (int)compressedSize, MPI_BYTE, &status);      
       
        if (error != MPI_SUCCESS ){   
                ERROR("Error writing to file");        
                return false;
        }
        
        return true;
            
       
    }
    
    LastWrittenChunk* DifferentialWriter::getLastBlock (int pid){
        
         for(std::vector<LastWrittenChunk*>::size_type i = 0; i !=lastChunks.size(); i++){            
             LastWrittenChunk *lastChunk = lastChunks[i];            
             if(lastChunk->pid == pid)
                 return lastChunk;
         }
         return NULL;
    }
}
