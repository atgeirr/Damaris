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
#ifndef __DAMARIS_DYNAMIC_ACTION_H
#define __DAMARIS_DYNAMIC_ACTION_H

#include <dlfcn.h>

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/action/BoundAction.hpp"
#include "damaris/action/GroupAction.hpp"

namespace damaris {

/**
 * DynamicAction is a child class of Action used for actions located in the
 * executable or in dynamic libraries (.so on Linux, .dylib on MacOSX). 
 * The dynamic library is loaded only the first time the action is called, 
 * thus avoiding unnecessary memory usage.
 */
class DynamicAction : public BoundAction, public Configurable<model::Event> {
	
	friend class Deleter<DynamicAction>;

	private:
	
	void* handle_; /*!< Handle for the opened dynamic library. */
	bool loaded_; /*!< Tells if the function is loaded or not. */

	protected:
		
	/**
	 * Condtructor.
	 * This constructor is protected and can only be called by the 
	 * ActionManager (friend class), because it does not provide 
	 * the action's ID and name.
	 * 
	 * \param[in] mdl : model of the event, from XML file.
	 */	
	DynamicAction(const model::Event& mdl)
	: BoundAction(mdl.name(), mdl.execution(), mdl.scope()), 
	  Configurable<model::Event>(mdl),
	  handle_(NULL), loaded_(false)
	{}
		
	/**
	 * Loads the function from the shared library or from the main program.
	 */
	virtual void Load();

	/**
	 * Destructor.
	 */
	virtual ~DynamicAction() {
		if(handle_ != NULL) dlclose(handle_);
	}
	
	public:	
		
	/**
	 * \see damaris::Action::operator()
	 */
	virtual void Call(int32_t sourceID, int32_t iteration,
				const char* args = NULL);

	/**
	 * Return true if the action can be called from outside the 
	 * simulation (typically from VisIt).
	 */
	virtual bool IsExternallyVisible() const { 
		return GetModel().external();
	}

	/**
	 * Create a DynamicAction given a model.
	 * 
	 * \param[in] ev : model from the XML file.
	 */
	template<typename SUPER>
	static shared_ptr<SUPER> New(const model::Event& ev, 
					const std::string& name) {
		shared_ptr<SUPER> a;
		if(ev.scope() != model::Scope::group) {
			a = shared_ptr<SUPER>(
					new DynamicAction(ev),
					Deleter<DynamicAction>());

		} else {
			a = GroupAction<DynamicAction,
					model::Event>::New(ev,name);
		}
		return a;
	}
};

}

#endif
