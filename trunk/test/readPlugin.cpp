
#include <stdint.h>
#include <iostream>
#include <string>

#include "core/VariableManager.hpp"
#include "core/Environment.hpp"
#include "storage/Writer.h"
#include "storage/StorageManager.h"


extern "C" {
void readVar(const std::string& eventName, int32_t step, 
	int32_t source, const char* args) {
    
        Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
	Damaris::Reader* w = Damaris::StorageManager::GetReaderFor(v, Damaris::Environment::GetMagicNumber());  
        //Damaris::DataSpace* dataSpace;
        std::vector<Damaris::DataSpace*> dataSpaceVector;
        
        //int iteration = atoi (&args[0]);
        //TODO: test it: w->Read(iteration,chunk)       
       
        //dataSpace=w->Read(1);
        
        dataSpaceVector=w->Read(0);
        
        if(dataSpaceVector.empty()==true)
            std::cout<<"eroare la citire"<<std::endl;
        else
            std::cout<<"success"<<std::endl;
        
        for (std::vector<Damaris::DataSpace*>::iterator it = dataSpaceVector.begin() ; it != dataSpaceVector.end(); ++it){
            ;
        }
        /*if(dataSpaceVector ==  NULL)
            std::cout<<"eroare la citire"<<std::endl;
        else
            std::cout<<"success"<<std::endl;*/
      
}
}

