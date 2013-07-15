/* 
 * File:   DifferentialWriter.cpp
 * Author: cata
 * 
 * Created on July 5, 2013, 5:36 PM
 */

#include "DifferentialWriter.hpp"
#include "zlib.h"
#include "data/Variable.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include <boost/filesystem.hpp>

namespace Damaris{
    DifferentialWriter::DifferentialWriter(Variable* v){
    
        this->var = v;
        this->lastIteration = 0;
        /*Create the file directory*/
        std::string path = getPath();
        boost::filesystem::create_directories(boost::filesystem::path(path.c_str()));
        
        /*Open file*/   
        std::ostringstream processID;   
        processID<<Process::Get()->getID();
        std::string fileName = path + "/" + processID.str();

        int error=MPI_File_open(MPI_COMM_SELF, fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);

        if (error != MPI_SUCCESS)
            ERROR("Error opening file");
    }

    DifferentialWriter::DifferentialWriter(const DifferentialWriter& orig) {
    }

    DifferentialWriter::~DifferentialWriter() {
         MPI_File_close(&damarisFile);
    }
    
    bool DifferentialWriter::Write(int iteration){
        return true; 
    }
    
    bool DifferentialWriter::Write(){
        
        MPI_Status status;
        ChunkIndexByIteration::iterator begin,end, it;    
        Chunk *chunk ;
        int error, typeSize,iteration;
        unsigned int dimensions;
        
        if (Environment::GetLastIteration()<lastIteration){        
            WARN("You are trying to write the same iteration twice");
            return false;
        } 
        
        this->var->GetChunksByIteration(lastIteration,begin,end);
        
       
        for (it=begin; it != end; it++) {
            DifferentialChunk chunkInfo;
            chunk = it->get();
            
            dimensions = chunk->NbrOfItems();
            iteration = chunk->GetIteration();        
            Model::Type t = this->var->GetLayout()->GetType();
            typeSize = Types::basicTypeSize(t);  
            
            int pid = chunk->GetSource();
            int bid = chunk->GetBlock();
            
            int compressedSize = compressBound(dimensions*typeSize);
            
            if (lastIteration == 0)
                createChunkStructure(chunkInfo,pid,iteration,compressedSize,bid,dimensions*typeSize);  
            else
                createChunkStructure(chunkInfo,pid,iteration,dimensions*typeSize,bid,dimensions*typeSize); 
            
            error=MPI_File_write(damarisFile, &chunkInfo, sizeof (ChunkInfo), MPI_BYTE, &status);
           
            if (error != MPI_SUCCESS ){   
                ERROR("Error writing to file");        
                return false;
            }
            //TODO: check the errors;
            if (lastIteration != 0)
                writeXorData (chunk, pid, (uLongf) compressedSize);
            else{
               
                void* data = chunk->GetDataSpace()->Data();
                error=MPI_File_write(damarisFile, data, typeSize*dimensions, MPI_BYTE, &status); 
            }
            
            
            
        }
        
        lastChunks.clear();
       
        for (it = begin; it!=end;it++){
           
            chunk = it->get();
            LastWrittenChunk* last = (LastWrittenChunk*)malloc (sizeof(LastWrittenChunk));
            last->size = chunk->GetDataSpace()->Size();
            last->data = chunk->GetDataSpace()->Data();
            last->pid = chunk->GetSource();
            lastChunks.push_back(last);
        } 
       lastIteration++;        
       return true;
    }
    
    void DifferentialWriter::createChunkStructure(DifferentialChunk &chunkInfo,int pid,int iteration,int size, int bid, int uncompressedSize){
        chunkInfo.pid = pid;
        chunkInfo.iteration = iteration;
        chunkInfo.size = size;
        chunkInfo.bid = bid;
        chunkInfo.uncompressedSize = uncompressedSize;
    }
    
    std::string DifferentialWriter::getPath(){
        std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
        return path ;
    } 
   
    //write and compress the xor between two chunks
    void DifferentialWriter::writeXorData (Chunk* chunk, int pid, uLongf sizeDest){
        MPI_Status status;
       
        Bytef* data = (Bytef*) chunk->GetDataSpace()->Data();
      
        LastWrittenChunk* last = getLastBlock(pid);
       
        Bytef* data1 = (Bytef*)last->data;
       
        int size = chunk->GetDataSpace()->Size();
        Bytef* newData = (Bytef*)malloc(size);
       
        for (int i=0; i<size;i++){
            newData[i] = data[i] ^ data1[i];
        }
        Bytef* dest = (Bytef*)malloc(size*sizeof(Bytef));
        int error = compress2(dest, &sizeDest, (Bytef *) newData, size, 9);
        
        if(error!= Z_OK)
            ERROR ("Error on compression");
        
        error=MPI_File_write(damarisFile, dest, sizeDest, MPI_BYTE, &status);     
            
       
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