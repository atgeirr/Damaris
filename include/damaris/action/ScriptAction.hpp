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
#ifndef __DAMARIS_SCRIPT_ACTION_H
#define __DAMARIS_SCRIPT_ACTION_H

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/action/Action.hpp"
#include "damaris/data/Variable.hpp"


// #undef __ENABLE_PYTHON // TODO

#ifdef HAVE_PYTHON_ENABLED
#include "damaris/scripts/PyAction.hpp"
#endif

namespace damaris {

/**
 * ScriptAction describes an Action that wraps an interpretable script,
 * such as a Python script. This class is virtual and inherited, for
 * example, by PyAction.
 */
class ScriptAction : public Action, public Configurable<model::Script> {

    protected:
    /**
     * Condtructor.
     */
    ScriptAction(const model::Script& mdl)
    : Action(mdl.name()), Configurable<model::Script>(mdl)
    { }

    /**
     * Destructor.
     */
    virtual ~ScriptAction() {}

    public:    
    
    /**
     * \see damaris::Action::operator()
     */
    virtual void Call(int32_t sourceID, int32_t iteration,
                const char* args = NULL) {} ;

    /**
     * Tells if the action can be called from outside the simulation.
     */
    virtual bool IsExternallyVisible() const { 
        return GetModel().external(); 
    }

    /**
     * \see Action::GetExecLocation
     */
    virtual model::Exec GetExecLocation() const {
        return GetModel().execution();
    }

    /**
     * \see Action::GetScope
     */
    virtual model::Scope GetScope() const {
        return GetModel().scope();
    }

    /**
     * Creates a new instance of an inherited class of ScriptAction 
     * according to the "language" field in the description.
     */
    template<typename SUPER>
    static std::shared_ptr<SUPER> New(const model::Script& mdl, 
                    const std::string& name) {
        return std::shared_ptr<SUPER>();
    }

    
    template<typename SUPER>
    static std::shared_ptr<SUPER> New(const model::Script& mdl)
    {
        return New<SUPER>(mdl,mdl.name());
    }
    
};

}

#endif
