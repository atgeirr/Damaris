#include <iostream>

extern "C" {

void my_function(const std::string& event, int32_t step, int32_t src, const char* args)
{
        std::cout << "--- hello world from Damaris ---" << std::endl;
}

}
