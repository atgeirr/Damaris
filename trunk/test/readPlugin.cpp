
#include <stdint.h>
#include <iostream>
#include <string>

#include "core/VariableManager.hpp"
#include "storage/Writer.h"
#include "storage/StorageManager.h"


extern "C" {
void readVar(const std::string& eventName, int32_t step, 
	int32_t source, const char* args) {
    
        Damaris::Variable* v = Damaris::VariableManager::Search("my_group/my_variable");
	Damaris::Reader* w = Damaris::StorageManager::GetReaderFor(v);  
        Damaris::DataSpace* dataSpace;
        //TODO: test it: w->Read(iteration,chunk)       
       
        dataSpace=w->Read(1);
        
        if(dataSpace ==  NULL)
            std::cout<<"eroare la citire"<<std::endl;
        else
            std::cout<<"success"<<std::endl;
      
}
}

