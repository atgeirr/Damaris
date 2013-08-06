/* 
 * File:   Reader.cpp
 * Author: cata
 * 
 */

#include "storage/Reader.hpp"
#include "core/Debug.hpp"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"
#include "storage/StorageManager.hpp"

namespace Damaris {
 void Reader::initFile(std::string magicNumber){
       
       
        std::string path = getPath(magicNumber);
        std::ostringstream processID; 
                
        processID<<Process::Get()->getID();    
        std::string fileName = path + "/" + processID.str();    
        int error=MPI_File_open(MPI_COMM_SELF, (char*)fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);

        if (error != MPI_SUCCESS){            
                ERROR("Error opening file");
                exit(0);
        }
  }

  std::string Reader::getPath(std::string magicNumber){
        std::string path = StorageManager::basename + "/"+ magicNumber + "/" + this->var->GetName();    
        return path;
                
  } 
  
}