
#include <stdint.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <functional>
#include "storage/StorageManager.hpp"
#include "core/Debug.hpp"
#include "core/VariableManager.hpp"
#include "core/Environment.hpp"
#include "zlib.h"


extern "C" {
void compareIterations(const std::string& eventName, int32_t step, 
	int32_t source, const char* args) {
    
   
    
    long int magicNumber = Damaris::StorageManager::GetPreviousMagicNumber();
    int ok=1;
    
    //convert from long int to string
    std::stringstream ss;
    ss << magicNumber;  
    std::string previousIteration=ss.str(); 
    
    Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
    Damaris::Reader* previousRun = Damaris::StorageManager::GetReaderFor(v, previousIteration);
    Damaris::Reader* currentRun = Damaris::StorageManager::GetReaderFor(v, Damaris::Environment::GetMagicNumber());
      
    std::map<int,Damaris::DataSpace*> previousDataSpaceVector;
    std::map<int,Damaris::DataSpace*> dataSpaceVector;
    
     
    previousDataSpaceVector=previousRun->Read(step);  
    dataSpaceVector = currentRun->Read(step);    
  
    if(previousDataSpaceVector.empty()==true || dataSpaceVector.empty()==true){
        WARN("Reading Error");
        exit(0);
    }
    
        
   for(std::map<int,Damaris::DataSpace*>::iterator i = dataSpaceVector.begin(); i != dataSpaceVector.end(); i++) {
       ok=1;
       int key = i->first;
       std::map<int,Damaris::DataSpace*>::iterator it = previousDataSpaceVector.find(key);
      
       Damaris::DataSpace* dataSpace = i->second;
       Damaris::DataSpace* previousDataSpace =it->second;
       
       void* data = dataSpace->Data();
       void* previousData= previousDataSpace->Data();     
      
       if (dataSpace->Size()!= previousDataSpace->Size())
           WARN ("Different size read");
       
      for(size_t j=0;j<dataSpace->Size();j++){
        
         Bytef* a = (Bytef*)data;
         Bytef* b = (Bytef*)previousData;
         
              
         if(*(a+j)!=*(b+j)){
              ok=0;             
              break;
         }
          
        }
        
       
   }
   
   if(ok==0){
       WARN("Differences found");
       exit(0); //drop the simulation
   }
   else
       std::cout<<"Success"<<std::endl;
}
}



