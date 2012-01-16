#include <stdio.h>

extern "C" {

void visit(const std::string event, int32_t step, int32_t src)
{
	printf("--- hello world from Damaris ---\n");
}

}
