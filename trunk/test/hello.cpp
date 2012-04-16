#include <stdint.h>
#include <iostream>
#include <string>

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
	}
}
