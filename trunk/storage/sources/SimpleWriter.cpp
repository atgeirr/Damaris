/* 
 * File:   SimpleWriter.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 2:56 PM
 */

#include "core/Debug.hpp"
#include "storage/SimpleWriter.hpp"
#include "data/Variable.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include <boost/filesystem.hpp>

namespace Damaris{
    

SimpleWriter::SimpleWriter(Variable* v) {
        
    this->var = v;
    this->lastIteration = 0;
    initFile();
   
    
}

SimpleWriter::SimpleWriter(const SimpleWriter& orig) {
}

SimpleWriter::~SimpleWriter() {
    MPI_File_close(&damarisFile);
}

bool SimpleWriter::Write() {

    
    int typeSize, iteration,error,pid;
    unsigned int dimensions;   
    MPI_Status status;
    ChunkIndexByIteration::iterator begin,end, it;    
    
    if (Environment::GetLastIteration()<lastIteration){        
        WARN("You are trying to write the same iteration twice");
        return false;
    }     
    this->var->GetChunksByIteration(lastIteration,begin,end);
   
    Damaris::Chunk *chunk ;
    for (it=begin; it != end; it++) {
       
        ChunkInfo chunkInfo;
        chunk = it->get();      
        //std::cout<<chunk->GetSource()<<" "<<chunk->GetIteration();
        dimensions = chunk->NbrOfItems();
        iteration = chunk->GetIteration(); 
        pid = chunk->GetSource();
        Model::Type t = this->var->GetLayout()->GetType();
        typeSize = Types::basicTypeSize(t);             
        createChunkStructure(chunkInfo,this->var->GetID(), iteration, dimensions*typeSize,pid);        
        error=MPI_File_write(damarisFile, &chunkInfo, sizeof (ChunkInfo), MPI_BYTE, &status);
        //std::cout<<"Writing it"<<chunkInfo.iteration<<" "<<chunkInfo.size<<" ";
        if (error != MPI_SUCCESS ){   
            ERROR("Error writing to file");        
            return false;
        }
        
        error=MPI_File_write(damarisFile, chunk->GetDataSpace()->Data(), typeSize*dimensions, MPI_BYTE, &status);        
       
        if (error != MPI_SUCCESS ){   
            ERROR("Error writing to file");        
            return false;
        }
       

    }
    
    lastIteration++;      
    return true;
}

bool SimpleWriter::Write(int interation){
   return true; 
}

void SimpleWriter::createChunkStructure(ChunkInfo &chunkInfo,int id, int iteration, int size,int pid) {  
    chunkInfo.id = id;
    chunkInfo.iteration = iteration;
    chunkInfo.size = size;   
    chunkInfo.pid = pid;
}

/*std::string SimpleWriter::getPath() {  
    std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
    return path ;
}*/

}







