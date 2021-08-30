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

#ifndef __DAMARIS_OBSERVER_H
#define __DAMARIS_OBSERVER_H

#include <set>

#include "damaris/util/Pointers.hpp"
#include "damaris/util/ForwardDcl.hpp"

namespace damaris {

/**
 * Observer represents an object that needs to be notified of
 * any changes of some Observable objects.
 */
class Observer {
	friend class Observable;

		std::set<Observable*> observed_; 
		/*!< Set of Observable that need to notify this 
		object of their changes. This set intentionally uses
		raw pointers. */

	protected:

		/**
		 * This function needs to be overwritten by the child classes 
		 * such that they can take proper dedictions when an observed 
		 * Observable changes.
		 */
		virtual void Notify() = 0;
	
		/**
		 * This function is called when an observable is deleted.
		 * It is intentionally not virtual so that the child classes
		 * cannot override it (could lead to bad behaviors).
		 * It uses a raw pointer instead of a smart pointer because
		 * it will be called from the destructor of the object, thus
		 * no smart pointer to this object exist anymore.
		 *
		 * \param[in] obs : Observable that is destroyed.
		 */
		void NotifyDestruction(Observable* obs);

	public:
		/**
		 * This destructor disconnects properly from all the observed 
		 * objects such that they do not notify an inexiting object.
		 */
		virtual ~Observer();
};

} // namespace Damaris

#endif
