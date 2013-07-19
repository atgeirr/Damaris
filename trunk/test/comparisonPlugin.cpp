
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
    Damaris::Reader* currentRun = Damaris::StorageManager::GetReaderFor(v,Damaris::Environment::GetMagicNumber());
    std::vector<Damaris::DataSpace*> previousDataSpaceVector;  
    std::vector<Damaris::DataSpace*> dataSpaceVector;      
        
    previousDataSpaceVector=previousRun->Read(step);
    dataSpaceVector = currentRun->Read(step);
    
    if(previousDataSpaceVector.empty()==true || dataSpaceVector.empty()==true){
        WARN("Eroare la citire");
        exit(0);
    }
    
        
   for(std::vector<Damaris::DataSpace*>::size_type i = 0; i != dataSpaceVector.size(); i++) {
       ok=1;
       Damaris::DataSpace* dataSpace = dataSpaceVector[i];
       Damaris::DataSpace* previousDataSpace = previousDataSpaceVector[i];
       
       void* data = dataSpace->Data();
       void* data1= previousDataSpace->Data();     
      
       if (dataSpace->Size()!= previousDataSpace->Size())
           std::cout<<"different size"<<std::endl;
     
      for(size_t j=0;j<dataSpace->Size();j++){
          // TODO: generic cast
         Bytef* a = (Bytef*)data;
         Bytef* b = (Bytef*)data1;
        
          //some random output for control
          /*if (j==2){
              std::cout<<*(a+j)<<std::endl;
              std::cout<<*(b+j)<<std::endl;
          }
           */
        
         if(*(a+j)!=*(b+j)){
              ok=0;
              std::cout<<j<<" "<<std::endl;
              std::cout<<*(a+j)<<std::endl;
              std::cout<<*(b+j)<<std::endl;
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



