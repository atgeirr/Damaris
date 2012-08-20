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
 * \file Observer.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_OBSERVER_H
#define __DAMARIS_OBSERVER_H

#include <set>

namespace Damaris {

class Observable;

/**
 * Observer represents an object that needs to be notified of
 * any changes of some Observable objects.
 */
class Observer {
	friend class Observable;

		std::set<Observable*> observed; /*!< Set of Observable that need to notify this object of their changes. */

	protected:

		/**
		 * This function needs to be overwritten by the child classes such that
		 * they can take proper dedictions when an observed Observable changes.
		 */
		virtual void Notify() = 0;

	public:
		/**
		 * This destructor disconnects properly from all the observed objects
		 * such that they do not notify an inexiting object.
		 */
		virtual ~Observer(); 
};

} // namespace Damaris

#endif
