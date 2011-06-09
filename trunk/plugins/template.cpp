#include <stdio.h>
#include "common/MetadataManager.hpp"

extern "C" {

void my_function(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{
	printf("--- hello world from Damaris ---\n");
}

}
