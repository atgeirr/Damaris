/* 
 * File:   StorageManager.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 1:02 PM
 */

#include "StorageManager.h"
#include "mpi.h"
#include "SimpleWriter.h"
#include "SimpleReader.h"


namespace Damaris {

std::string StorageManager::basename;
void StorageManager::Init(const Model::Storage& s)
{
    basename = s.basename();
  
}

Reader* StorageManager::GetReaderFor(Variable* v)
{
    Reader* reader = new SimpleReader(v); // TODO create an instance of SimpleReader
    
    return reader;
    
}

Writer* StorageManager::GetWriterFor(Variable* v)
{
    Writer* w = new SimpleWriter(v); // TODO create an instance of SimpleWriter
    
    return w;
}



}
