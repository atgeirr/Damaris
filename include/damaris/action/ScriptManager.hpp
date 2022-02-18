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
#ifndef __DAMARIS_SCRIPT_MANAGER_H
#define __DAMARIS_SCRIPT_MANAGER_H

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/action/Action.hpp"

namespace damaris {

/**
 * ScriptManager is a class that offers an access by name and by id to all 
 * scripts managed by the program. It inherites from Manage<Action>.
 */
class ScriptManager : public Manager<Action> {
private:
//     model::Actions  mdl_;
public:
    
    /**
     * Initialize the ScriptManager with a model: goes through all the 
     * events and scripts described in the XML file, creates the appropriate
     * Actions instances, and stores them.
     */
    static void Init(const model::Scripts& mdl_script);
    
    /**
     * Loop through and execute the actions in the ScriptManager
     * 
     * Called in src/server/Server.cpp after damaris_end_iteration() is called
     */
    static bool RunScripts(const int iteration);   // static bool RunActions(const int iteration);
};

}

#endif
