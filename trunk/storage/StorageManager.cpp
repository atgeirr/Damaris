/* 
 * File:   StorageManager.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 1:02 PM
 */

#include "StorageManager.hpp"
#include "mpi.h"
#include "SimpleReader.hpp"
#include "SimpleWriter.hpp"
#include "DifferentialReader.hpp"
#include "DifferentialWriter.hpp"
#include "core/Environment.hpp"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>


namespace Damaris {

std::string StorageManager::basename;
std::map<int,Writer*> StorageManager::writersMap;
std::map<int,Reader*> StorageManager::readersMap;

void StorageManager::Init(const Model::Storage& s)
{
    basename = s.basename();
  
}

Reader* StorageManager::GetReaderFor(Variable* v,std::string magicNumber)
{
    Reader* reader;
    std::map<int,Reader*>::iterator it = readersMap.find(v->GetID());
    
    if(it!=readersMap.end()){
       return it->second;
    }
    reader = new SimpleReader(v,magicNumber);
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

long int StorageManager::GetPreviousMagicNumber(){
    
    boost::filesystem::path myPath(StorageManager::basename.c_str());
    boost::filesystem::directory_iterator end_iter;
    std::string numbers="0123456789";
    long int maxNr = 0;
    int ok = 1;
    
    for(boost::filesystem::directory_iterator dir_iter(myPath) ; dir_iter != end_iter ; ++dir_iter){
        if(boost::filesystem::is_directory(dir_iter->path())){
             ok = 1;
             std::string str = dir_iter->path().string();
             std::string subst = str.substr(StorageManager::basename.size()+1,str.size()-StorageManager::basename.size());
             char *cstr = new char[numbers.length() + 1];
             strcpy(cstr, numbers.c_str());
             char *dirPath = (char*)subst.c_str();
             
             for (unsigned int i=StorageManager::basename.size();i<subst.size();i++){
                 char* pos = strchr(cstr,dirPath[i]);                 
                 if(pos==NULL)
                     ok=0;         
             }
             if(ok==1 && atol(subst.c_str())!=atol(Environment::GetMagicNumber().c_str())){                 
                 maxNr = std::max(atol(subst.c_str()),maxNr);
             }
             
             delete [] cstr;
        }         
    }
    return maxNr;
    
} 

}
