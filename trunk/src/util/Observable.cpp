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

#include "util/Observer.hpp"
#include "util/Observable.hpp"
#include "util/Debug.hpp"

namespace damaris {

void Observable::Changed() const
{
	std::set<weak_ptr<Observer> >::iterator it = observers_.begin();
	for(;it != observers_.end(); it++) {
		shared_ptr<Observer> p = it->lock();
		if(p) p->Notify();
	}
}

} // namespace damaris

