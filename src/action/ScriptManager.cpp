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
#include "util/Debug.hpp"
#include "action/ScriptManager.hpp"
#include "action/DynamicAction.hpp"
//#include "action/ScriptAction.hpp"
#include "action/BoundAction.hpp"
#include "scripts/PyAction.hpp"
#include "gc/GarbageCollector.hpp"

namespace damaris {

void ScriptManager::Init(const model::Scripts& mdl_script)
{
    // find the handler's name
    std::string handler_name;
    bool handler_is_event = true;

    model::Scripts::pyscript_const_iterator s(mdl_script.pyscript().begin());
    for(; s != mdl_script.pyscript().end(); s++) {
        Create<PyAction>(*s);  // (std::string)s->name() this is actually a cal to PyAction::New(mdl)
        std::string file_ = s->file() ;
        std::cout << "ScriptManager has found a script which has a file field named: " << file_ << std::endl ;
        if(!handler_is_event && (std::string)s->name() == handler_name) 
        {
            Create<PyAction>(*s,"#error");
        }
    }

    
  // mdl_= mdl ;
}


// was RunActions(
bool ScriptManager::RunScripts(const int iteration)
{
    
    iterator s = Begin();
    for(; s != End(); s++) {
        (*s)->Call(iteration, iteration);
    }
    
    /*
    // loop through actions looking for the script() actions
    auto actnItr = Begin();
    for(; actnItr != End(); actnItr++) {
        std::shared_ptr<Action> action = *actnItr;
       // action->CoProcess(iteration) ;
        const model::Actions& actn_mdl = GetModel(); 
       if (actn_mdl.language().present() ) {
           
           std::string file_ = Actions.get().file() ;
           std::cout << "RunActions has found a script which has a file field named: " << file_ << std::endl ;
       }
    }
    */
    return true ;
} // end of RunActions

}  // end damaris namespace
