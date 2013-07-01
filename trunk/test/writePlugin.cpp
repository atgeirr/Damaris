
#include <stdint.h>
#include <iostream>
#include <string>

#include "core/VariableManager.hpp"
#include "storage/Writer.h"
#include "storage/StorageManager.h"


extern "C" {
void writeVar(const std::string& eventName, int32_t step, 
	int32_t source, const char* args) {	
        Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
	Damaris::Writer* w = Damaris::StorageManager::GetWriterFor(v);          
        w->Write();   
      
}
}

