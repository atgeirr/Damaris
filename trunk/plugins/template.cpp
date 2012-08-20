#include <stdio.h>

extern "C" {

void my_function(const std::string& event, int32_t step, int32_t src, const char* args)
{
	printf("--- hello world from Damaris ---\n");
}

}
