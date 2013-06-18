/* 
 * File:   StorageManager.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 1:02 PM
 */

#include "StorageManager.h"
#include "mpi.h"
#include "SimpleReader.h"
#include "SimpleWriter.h"

namespace Damaris {

std::string StorageManager::basename;
std::map<int,Writer*> StorageManager::writersMap;
std::map<int,Reader*> StorageManager::readersMap;

void StorageManager::Init(const Model::Storage& s)
{
    basename = s.basename();
  
}

Reader* StorageManager::GetReaderFor(Variable* v)
{
    Reader* reader;
    std::map<int,Reader*>::iterator it = readersMap.find(v->GetID());
    
    if(it!=readersMap.end()){
       return it->second;
    }
    reader = new SimpleReader(v);
    readersMap[v->GetID()] = reader;
    
    return reader;
    
}

Writer* StorageManager::GetWriterFor(Variable* v)
{
    Writer* writer;
    std::map<int,Writer*>::iterator it = writersMap.find(v->GetID());
    
    if(it!=writersMap.end()){      
      return it->second;      
    }
    writer = new SimpleWriter(v);
    writersMap[v->GetID()] = writer;
   
    return writer; 
    
   
}



}
