
#include "data/VariableManager.hpp"

using namespace damaris;

// function called as a Damaris plugin
extern "C" void my_function(const char* /*name*/, int source, int iteration,
				const char* /*args*/) {
    std::shared_ptr<Variable> v = VariableManager::Search("life/cells");
	
	if(v) printf("Variable found, ");
	else return;
	
	printf("iteration = %d, source = %d, ", iteration, source);
	
    std::shared_ptr<Block> b = v->GetBlock(source, iteration, 0);
	if(b) printf("block found, writable = %d\n",not b->IsReadOnly());
	else printf("\n");
}

extern "C" void called_locally(const char* name, int source, int iteration,
				const char* /*args*/) {
	printf("Event %s called on process %d\n",name,source);

}

extern "C" void bcasted_event(const char* name, int source, int iteration,
				const char* /*args*/) { 
	printf("Event bcasted from source %d\n",source);
}
