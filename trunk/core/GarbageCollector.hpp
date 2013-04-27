/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file GarbageCollector.hpp
 * \date April 2013
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_GC_H
#define __DAMARIS_GC_H

#include "event/Action.hpp"
#include "core/VariableManager.hpp"

namespace Damaris {

/**
 * Defines the default action when clients do not manage to
 * complete a write phase.
 */
class GarbageCollector  : public Action {
	public:
		/**
		 * \brief Constructor. 
		 */
		GarbageCollector() : Action("#error") {}
		
		virtual void Call(int32_t iteration, int32_t sourceID, const char *args = NULL)
		{
			VariableManager::iterator it = VariableManager::Begin();
			VariableManager::iterator end = VariableManager::End();
			while(it != end) {
				it->get()->Clear(iteration);
				it++;
			}
		}

		/**
		 * Indicates if the Action can be called from outside the simulation.
		 */
		virtual bool IsExternallyVisible() {
			return false;
		}

		virtual ~GarbageCollector() {}

};

}

#endif
