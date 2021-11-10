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
#include "action/ActionManager.hpp"
#include "action/DynamicAction.hpp"
#include "action/ScriptAction.hpp"
#include "action/BoundAction.hpp"
#include "gc/GarbageCollector.hpp"

namespace damaris {

void ActionManager::Init(const model::Actions& mdl)
{
	// find the handler's name
	std::string handler_name;
	bool handler_is_event = true;

	model::Actions::error_const_iterator r(mdl.error().begin());
	if(r != mdl.error().end()) {
		// build the action for error handling
		if(r->event().present() && r->script().present()) {
			WARN("Error handler should be attached to either "
			<< "an event or a script.");
		}
		if(r->event().present()) {
			handler_name = r->event().get();
			handler_is_event = true;
		}
		if(r->script().present()) {
			handler_name = r->script().get();
			handler_is_event = false;
		}
		r++;
		if(r != mdl.error().end()) {
			WARN("Multiple definition of error handlers, "
			<< "only the first one will be registered.");
		}
	} else {
		Add(GarbageCollector::New());
	}

	// build events
	model::Actions::event_const_iterator e(mdl.event().begin());
	for(; e != mdl.event().end(); e++) {
		Create<DynamicAction>(*e,(std::string)e->name());
		if(handler_is_event && ((std::string)e->name() == handler_name))
		{
			Create<DynamicAction>(*e,"#error");
		}
	}

	// build scripts
	model::Actions::script_const_iterator s(mdl.script().begin());
	for(; s != mdl.script().end(); s++) {
		Create<ScriptAction>(*s,(std::string)s->name());
		if(!handler_is_event && (std::string)s->name() == handler_name) 
		{
			Create<ScriptAction>(*s,"#error");
		}
	}
	
	if(not Search("#error")) {
		WARN("Error handler not found, switching back to default"
		<< " garbage collector.");
		Add(GarbageCollector::New());
	}
	
//mdl_= mdl ;
}

bool ActionManager::RunActions(const int iteration)
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
      //const model::Actions& actn_mdl = action->GetModel();  
       
       if (action::language().present() ) {
           
           std::string file_ = actn_mdl.get().file() ;
           std::cout << "RunActions has found a script which has a file field named: " << file_ << std::endl ;
       }
     //
       
       */
 //  model::Actions::script_sequence scripts = mdl_.get().script();
 //      model::Actions::script_const_iterator it ;

 //   for(it = scripts.begin(); it != scripts.end(); ++it) {
       
       //actnmdl
       // actnmdl::action_const_iterator s(mdl.store().begin());   
 //   }

   // }
    
    return true ;
} // end of RunActions

}  // end damaris namespace

/*
	
	 * This function creates an object of type SUBCLASS (which must be a 
	 * subclass of the class T) by calling its "New" function with a MODEL 
	 * parameter. The object is stored in the multiindex structure and a 
	 * pointer to it is returned. If another object with the same name
	 * already exists, the object is not created but the old object is 
	 * returned instead. A configuration warning is output on stderr.
	 * 
	 * Example: in a Manager<Action>, one can use
	 * Manager<Action>::Create<DynamicAction>(Model::Event mdl)
	 * since DynamicAction inherites from Action and has a constructor that
	 * takes a Model::Event instance.
	 *
	 * \param[in] mdl : model from which to create the object.
	 * \param[in] name : name to give to the object (if it needs to be renamed).
	 *
	template<typename SUBCLASS, typename MODEL>
	static std::shared_ptr<T> Manager::Create(const MODEL &mdl, const std::string& name)
	{
		std::shared_ptr<T> t(
		SUBCLASS::template New<T>(mdl,name));
		if(not (bool)t) return t;

		t->id_ = GetNumObjects();
		//t->name_ = name;

		std::pair<typename ObjectSet::iterator,bool> ret
			= _objects_.insert(t);
		if(ret.second == false) {
			CFGERROR("Duplicate element \""<< name << 
				"\" not created, " 
				<< "returning previous value instead.");
			// no need to delete t here, the shared_ptr does it for us
			return *(ret.first);
		}
		
		return t;
	}
	
	
		
	 * Creates a new instance of an inherited class of ScriptAction 
	 * according to the "language" field in the description.
	 
	template<typename SUPER>
	static std::shared_ptr<SUPER> ScriptAction::New(const model::Script& mdl, 
				     const std::string& name) {
		switch(mdl.scope()) {
		case model::Scope::core :
		case model::Scope::bcast :

			if(mdl.language() == model::Language::python) {
#ifdef HAVE_PYTHON_ENABLED
				return PyAction::New(mdl,name);
#else
				CFGERROR("Damaris has not been compiled"
					<< " with Python support.");
#endif
			} else {
				CFGERROR("\"" << mdl.language() 
				<< "\" is not a valid scripting language.");
			}
			break;

		case model::Scope::group :
			if(mdl.language() == model::Language::python) {
#ifdef HAVE_PYTHON_ENABLED
				return NodeAction<Python::PyAction,
					model::Script>::New(mdl,name);
#else
				CFGERROR("Damaris has not been compiled"
				<< " with Python support.");
#endif
			} else {
				CFGERROR("\"" << mdl.language() 
				<< "\" is not a valid scripting language.");
			}
			break;
		}
		return std::shared_ptr<SUPER>();
	}
 * 
 * */
