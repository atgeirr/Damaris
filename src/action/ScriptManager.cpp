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

#ifdef HAVE_PYTHON_ENABLED
    model::Scripts::pyscript_const_iterator s(mdl_script.pyscript().begin());
    for(; s != mdl_script.pyscript().end(); s++) {
        std::string file_ = s->file() ;
        std::cout << "ScriptManager has found a script which has a file field named: " << file_ << std::endl ;
        Create<PyAction>(*s);  // (std::string)s->name() this is actually a call to PyAction::New(mdl)
        if(!handler_is_event && (std::string)s->name() == handler_name) 
        {
            Create<PyAction>(*s,"#error");
        }
    }
#endif  // HAVE_PYTHON_ENABLED

    VariableManager::iterator v = VariableManager::Begin();
    for(; v != VariableManager::End(); v++) {
        const model::Variable& mdl = (*v)->GetModel();
        if(mdl.script() == "#") continue;
        
        std::vector<std::string> scripts;
        boost::split(scripts, mdl.script(), boost::is_any_of(",; "));
        
        std::vector<std::string>::iterator s = scripts.begin();
        for(; s != scripts.end(); s++) {
            std::shared_ptr<Action> st = Search(*s); // Search is inherited from Manager
            if(st) {
                st->AddVariable(*v);
            } else {
                CFGERROR("Unknown script \""
                << *s << "\" for variable \"" 
                << (*v)->GetName() << "\"");
            }
        }
    }
    
  // mdl_= mdl ;
}


// was RunActions(
bool ScriptManager::RunScripts(const int iteration)
{
    /*
    iterator s = Begin();
    for(; s != End(); s++) {
        (*s)->Call(iteration, iteration);
    }
    */
    
    // loop through actions looking for the script() actions
    auto actnItr = Begin();
    for(; actnItr != End(); actnItr++) {
        
       // if ( dynamic_cast<PyAction*>(*actnItr) != nullptr ) {
#ifdef HAVE_PYTHON_ENABLED     
        if ( std::dynamic_pointer_cast<PyAction>(*actnItr) != nullptr ) {
            std::shared_ptr<Action> action = *actnItr;
            action->Call(iteration, iteration) ;            
        }
#endif  // HAVE_PYTHON_ENABLED
        
       // action->CoProcess(iteration) ;
       /* const model::Actions& actn_mdl = GetModel(); 
       if (actn_mdl.language().present() ) {
           
           std::string file_ = Actions.get().file() ;
           std::cout << "RunActions has found a script which has a file field named: " << file_ << std::endl ;
       }
       */
    }
    
    return true ;
} // end of RunScripts

}  // end damaris namespace
