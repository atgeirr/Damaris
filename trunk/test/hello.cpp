
#include <stdint.h>
#include <iostream>
#include <string>

#include "core/VariableManager.hpp"

// The following is an embedded plugin.
// Note the "extern C", used so Damaris can find the symboles correctly.
extern "C" {
	void hello_world(const std::string eventName, int32_t step, int32_t source) {
		std::cout << "Hello World! "
				  << "(event sent by client " << source
				  << " at iteration " << step << ")" << std::endl;
		// Getting access to the data :
		// Damaris::MetadataManager* mm = Damaris::MetadataManager::getInstance();
		// mm->getVariable(...) ... (see documentation)
//		Damaris::VariableManager::ForEach(
//				std::cout << boost::bind(&Damaris::Variable::getName,boost::lambda::_1) << std::endl
//		);
		Damaris::Variable* v = Damaris::VariableManager::Search("images/julia");
		if(v != NULL) std::cout << "youpi it works";
		else std::cout << "it doesn't";
	}
}
