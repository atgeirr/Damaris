//
// Created by hadi on 4/4/18.
//

#include <iostream>
#include <VariableManager.hpp>

using namespace damaris;

extern "C" {
void my_function(const std::string& event, int32_t step, int32_t src, const char* args)
{
    //std::cout << "--- Freeing memory for src " << src << " --- " ; //<< std::endl;
    // std::cout << "Environment iteration is " << Environment::GetLastIteration() << std::endl;

    if (src < 2 ) return;

    DBG("Erasing up to iteration " << step);
    DBG("Environment iteration is "
        << Environment::GetLastIteration());
    VariableManager::iterator it = VariableManager::Begin();
    VariableManager::iterator end = VariableManager::End();

    while(it != end) {
        // we don't erase non-time-varying data
        if (it->get()->IsTimeVarying()) {
            (*it)->ClearUpToIteration(src - 2);
        }
        it++;
    }
}
}

