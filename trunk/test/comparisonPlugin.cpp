
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
    
   
    std::cout<<"STEP: "<<step<<std::endl;
    long int magicNumber = Damaris::StorageManager::GetPreviousMagicNumber();
    int ok=1;
    
    //convert from long int to string
    std::stringstream ss;
    ss << magicNumber;  
    std::string previousIteration=ss.str(); 
    
    Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
    Damaris::Reader* previousRun = Damaris::StorageManager::GetReaderFor(v, previousIteration);
    Damaris::Reader* currentRun = Damaris::StorageManager::GetReaderFor(v, Damaris::Environment::GetMagicNumber());
    //std::vector<Damaris::DataSpace*> previousDataSpaceVector;  
    //std::vector<Damaris::DataSpace*> dataSpaceVector;      
    std::map<int,Damaris::DataSpace*> previousDataSpaceVector;
    std::map<int,Damaris::DataSpace*> dataSpaceVector;
    
    std::cout<<"Previous Read"<<std::endl;    
    previousDataSpaceVector=previousRun->Read(step);
    std::cout<<"Current Read"<<std::endl;
    dataSpaceVector = currentRun->Read(step);
    
    std::cout<<previousDataSpaceVector.size()<<" "<<dataSpaceVector.size()<<std::endl;
    if(previousDataSpaceVector.empty()==true || dataSpaceVector.empty()==true){
        WARN("Eroare la citire");
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
           std::cout<<"different size"<<std::endl;
       
      for(size_t j=0;j<dataSpace->Size();j++){
          // TODO: generic cast
         Bytef* a = (Bytef*)data;
         Bytef* b = (Bytef*)previousData;
         
              
         if(*(a+j)!=*(b+j)){
              ok=0;
              std::cout<<"J:"<<j<<" "<<std::endl;
              std::cout<<"A:"<<(int)*(a+j)<<std::endl;
              std::cout<<"B:"<<(int)*(b+j)<<std::endl;
              break;
         }
          
        }
        std::cout<<"compare"<<std::endl; 
       
   }
   
   if(ok==0){
       std::cout<<"Differences found"<<std::endl;
       exit(0); //drop the simulation
   }
   else
       std::cout<<"Success"<<std::endl;
}
}



