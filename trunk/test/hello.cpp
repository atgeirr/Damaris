
#include <stdint.h>
#include <iostream>
#include <string>

#include "core/VariableManager.hpp"

// The following is an embedded plugin.
// Note the "extern C", used so Damaris can find the symboles correctly.
extern "C" {
void hello_world(const std::string& eventName, int32_t step, 
	int32_t source, const char* args) {
	Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
	Damaris::Chunk* c = v->GetChunk(source,step);
	std::cout << "Chunk sent by process " << source << " is positioned at ["
		<< c->GetStartIndex(0) << ":" << c->GetEndIndex(0) << ","
		<< c->GetStartIndex(1) << ":" << c->GetEndIndex(1) << "]"
		<< std::endl;
}
}
