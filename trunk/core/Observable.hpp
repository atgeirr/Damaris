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
 * \file Observable.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_OBSERVABLE_H
#define __DAMARIS_OBSERVABLE_H

#include <set>

#include "core/Observer.hpp"

namespace Damaris {

/**
 * This class is used by child subclasses that need to notify
 * other objects that their internal stat has changed.
 */
class Observable {

	private:
		std::set<Observer*> observers; /*!< List of pointers to objects to notify. */

	protected:

		/**
		 * This function has to be called from any member function that modifies the
		 * Observable.
		 */
		void Changed() const;

	public:
		/**
		 * Adds an object to be notified of any changes in this Observable instance.
		 */
		void AddObserver(Observer* obs);

		/**
		 * Removes an object from the list of objects that observe this Observable.
		 */
		void RemoveObserver(Observer* obs);
};

} // namespace Damaris

#endif
