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
#ifndef __DAMARIS_ACTION_H
#define __DAMARIS_ACTION_H

#include <stdint.h>
#include <string>

#include "Damaris.h"

#ifdef HAVE_VISIT_ENABLED
#include <VisItDataInterface_V2.h>
#endif


#include "damaris/util/Pointers.hpp"
//#include "damaris/util/TypeWrapper.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Configurable.hpp"

#include "damaris/model/Model.hpp"

namespace damaris {

/**
 * The Action object is an interface to a user-defined action.
 * It is an abstract class, child classes must overwrite the Action::Call 
 * and Action::load functions.
 */
class Action {
    
    friend class Manager<Action>;
    friend class Deleter<Action>;

    std::string name_; /*!< Name of the action. */
    int id_;       /*!< ID given to the action when set in
                the ActionsManager. */
                
    std::vector<std::weak_ptr<Variable> > vars_; /*!< List of variables that will be accessible with this action/script */

    protected:
        
    /**
     * Constructor. 
     */
    Action(const std::string& n) : name_(n) {
#ifdef HAVE_PYTHON_ENABLED        
  //      Py_Initialize();
  //      np::initialize();
#endif
    }
            
    /**
     * Destructor.
     */
    virtual ~Action() {}
        
    public:
    
    /**
     * Gets the ID of the action.
     */        
    int GetID() const { return id_; }

    /**
     * Gets the name of the action.
     */
    const std::string& GetName() const { return name_; }    
    
    /**
     * This function is called on all variables that will be accessible
     * to the actions/scripts i.e. variables that have script="MyNamedScript" attribute
     * and MyNamedScript matches a script in the list of <scripts> ... </scripts> will be
     * added to vars_ vector by the ScriptManager class and their data will be exposed
     * to the script. 
     * ToDo: In the future we may want add a switch to allow all variables to be exposed.
     */
    void AddVariable(const std::shared_ptr<Variable>& v) {
        vars_.push_back(v);
    }

    /**
     * Returns a reference to the list of variables that have been reqested to be exposed
     * to the scripts/actions.
     */
     const std::vector< std::weak_ptr<Variable> >& GetVariables() const {
        return vars_;
    }


    /**
     * Operator overloaded to simplify the call to an action.
     * 
     * \param[in] source : id of the source that called the action.
     * \param[in] iteration : iteration at which the action is called.
     * \param[in] args : potential arguments.
     */
    void operator()(int32_t source, int32_t iteration, 
            const char *args = NULL);
        
    /**
     * Call the action. To be overloaded by child classes.
     * 
     * \param[in] source : id of the source that called the action.
     * \param[in] iteration : iteration at which the action is called.
     * \param[in] args : potential arguments.
     */
    virtual void Call(int32_t sourceID, int32_t iteration,
            const char *args = NULL) = 0;

    /**
     * Indicates if the Action can be called from outside the simulation.
     */
    virtual bool IsExternallyVisible() const = 0;

    /**
     * Returns the location of execution of this action.
     */
    virtual model::Exec GetExecLocation() const = 0;

    /**
     * Returns the scope of the action.
     */
    virtual model::Scope GetScope() const = 0;

#ifdef HAVE_VISIT_ENABLED
    /**
     * Expose the Action's metadata to VisIt.
     * \param[inout] md : an allocated visit_handle associated with a 
     * Command metadata.
     */
    bool ExposeVisItMetaData(visit_handle md);
#endif
};

}

#endif
