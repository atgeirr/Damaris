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
 */
class Observable {

	private:
		std::set<Observer*> observers;

	protected:
		void Changed() const;

	public:
		void AddObserver(Observer* obs);

		void RemoveObserver(Observer* obs);
};

} // namespace Damaris

#endif
