/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

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

