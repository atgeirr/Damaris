/* 
 * File:   SimpleWriter.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 2:56 PM
 */

#include "SimpleWriter.h"
#include "mpi.h"
#include "data/Variable.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include <boost/filesystem.hpp>

namespace Damaris{
    
int SimpleWriter::lastIteration = 0;

SimpleWriter::SimpleWriter(Variable* v) {
  
    this->var = v;
    
    //Create the file directory
     std::string path = getPath();
     boost::filesystem::create_directories(boost::filesystem::path(path.c_str())); 
}

SimpleWriter::SimpleWriter(const SimpleWriter& orig) {
}

SimpleWriter::~SimpleWriter() {
}

bool SimpleWriter::Write() {

    
    int typeSize, iteration,error;
    unsigned int dimensions;
    MPI_File damarisFile;
    MPI_Status status;
    

    ChunkIndexByIteration::iterator begin,end, it;

    /*Open file*/
    std::string path = getPath();    
    std::ostringstream processID;   
    processID<<Process::Get()->getID();
    std::string fileName = path + "/" + processID.str();
    
    error=MPI_File_open(MPI_COMM_SELF, fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);

    if (error != MPI_SUCCESS)
        return false;
    
    this->var->GetChunksByIteration(lastIteration,begin,end);
    
    for (it=begin; it != end; it++) {
        
        ChunkInfo chunkInfo;
        Damaris::Chunk *chunk = it->get();
        dimensions = chunk->NbrOfItems();
        iteration = chunk->GetIteration();
        Model::Type t = this->var->GetLayout()->GetType();
        typeSize = Types::basicTypeSize(t);
        Damaris::DataSpace* dataSpace = chunk->GetDataSpace();
        void* data = dataSpace->Data();      
       
        
        createChunkStructure(chunkInfo,this->var->GetID(), iteration, dimensions*typeSize);
        MPI_File_write(damarisFile, &chunkInfo, sizeof (ChunkInfo), MPI_BYTE, &status);
        MPI_File_write(damarisFile, data, typeSize*dimensions, MPI_BYTE, &status);
        
        if (status.MPI_ERROR != MPI_SUCCESS) {
            MPI_File_close(&damarisFile);
            return false;
        }

    }



    MPI_File_close(&damarisFile);
    return true;


}

bool SimpleWriter::Write(int interation){
   return true; 
}

void SimpleWriter::createChunkStructure(ChunkInfo &chunkInfo,int id, int iteration, int size) {
  
    chunkInfo.id = id;
    chunkInfo.iteration = iteration;
    chunkInfo.size = size;
    
}

std::string SimpleWriter::getPath() {  
    std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
    return path;
}

}







