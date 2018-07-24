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

#ifndef __DAMARIS_GC_H
#define __DAMARIS_GC_H

#include "damaris/util/Debug.hpp"
#include "damaris/util/Unused.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/action/Action.hpp"
#include "damaris/data/VariableManager.hpp"

namespace damaris {

/**
 * Defines the default action when clients do not manage to
 * complete a write phase.
 */
class GarbageCollector  : public Action {
	
	friend class Deleter<GarbageCollector>;
	
	/**
	 * Constructor.
	 */
	GarbageCollector() : Action("#error") {}
	
	/**
	 * Destructor.
	 */
	virtual ~GarbageCollector() {}
		
	public:
	
	/**
	 * \see Action::Call
	 * This function will delete all data posted for the given iteration,
	 * from all sources.
	 */
	virtual void Call(int32_t UNUSED(sourceID), int32_t iteration, 
		const char *args = NULL)
	{
		DBG("Erasing up to iteration " << iteration);
		DBG("Environment iteration is " 
			<< Environment::GetLastIteration());
		VariableManager::iterator it = VariableManager::Begin();
		VariableManager::iterator end = VariableManager::End();
		while(it != end) {
			// we don't erase non-time-varying data
			if(it->get()->IsTimeVarying()) {
				(*it)->ClearUpToIteration(iteration);
			}
			it++;
		}
	}

	/**
	 * Indicates if the Action can be called from outside the simulation.
	 */
	virtual bool IsExternallyVisible() const {
		return false;
	}

	/**
	 * \see Action::GetExecLocation
	 */
	virtual model::Exec GetExecLocation() const {
		return model::Exec::coupled;
	}

	/**
	 * \see Action::GetScope
	 */
	virtual model::Scope GetScope() const {
		return model::Scope::group;
	}
	
	/**
	 * Creates an action of type GarbageCollector.
	 */
	static std::shared_ptr<Action> New() {
		return std::shared_ptr<Action>(new GarbageCollector, 
			Deleter<GarbageCollector>());
	}
};

}

#endif
