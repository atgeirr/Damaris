/* 
 * File:   Writer.cpp
 * Author: cata
 *  */

/* 
 * File:   Reader.cpp
 * Author: cata
 * 
 */

#include "storage/Writer.hpp"
#include "core/Debug.hpp"
#include "data/Chunk.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "event/NodeAction.hpp"
#include "data/DataSpace.hpp"
#include "storage/StorageManager.hpp"
#include <boost/filesystem.hpp>
namespace Damaris {
    
void Writer::initFile(){
       
        
        std::string path = getPath();
        //create directory
        boost::filesystem::create_directories(boost::filesystem::path(path.c_str()));
        std::ostringstream processID;   
        processID<<Process::Get()->getID();
        std::string fileName = path + "/" + processID.str();    
       
        int error=MPI_File_open(MPI_COMM_SELF, (char*)fileName.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,MPI_INFO_NULL, &damarisFile);

        if (error != MPI_SUCCESS){            
                ERROR("Error opening file");
                std::cout<<error<<std::endl;
                exit(0);
        }
        
        
  }

std::string Writer::getPath() {  
    std::string path = StorageManager::basename + "/"+ Environment::GetMagicNumber() + "/" + this->var->GetName();    
    return path ;
}
  
}

