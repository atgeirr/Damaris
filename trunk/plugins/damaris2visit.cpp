#include "common/MetadataManager.hpp"

extern "C" {

void visit(const std::string event, int32_t step, int32_t src)
{
	Damaris::MetadataManager* mm = Damaris::MetadataManager::getInstance();
	// ...
}

}
