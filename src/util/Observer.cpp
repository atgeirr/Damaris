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

namespace damaris {

Observer::~Observer()
{
	std::set<Observable* >::iterator it 
		= observed_.begin();
	while(it != observed_.end()) {
		Observable* p = *it;
		p->RemoveObserver(this);
		it++;
	}
}

void Observer::NotifyDestruction(Observable* obs)
{
	observed_.erase(obs);
}

} // namespace damaris
