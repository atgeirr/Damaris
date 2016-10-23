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
#include "storage/NullStore.hpp"

namespace damaris {

NullStore::NullStore(const model::Store& mdl)
: Store(mdl) {
	model::Store::option_const_iterator op = mdl.option().begin();
	for(; op != mdl.option().end(); op++) {
		std::cout << "Option \"" << op->key() << "\" has value \"" << (string)(*op) << "\"" << std::endl;
	}
}

void NullStore::Output(int32_t iteration) {
	std::vector< weak_ptr<Variable> >::const_iterator w = GetVariables().begin();
	for(; w != GetVariables().end(); w++) {
		shared_ptr<Variable> v = w->lock();
		if(v) {
			std::cout << "Storging variable " << v->GetName() << " for iteration "
			<< iteration << std::endl;
		}
	}
}

}
