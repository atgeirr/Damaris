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
#include "data/ParameterManager.hpp"
#include "data/Type.hpp"


namespace damaris {

void ParameterManager::Init(const model::Data& mdl) 
{
	DBG("Initializing ParameterManager");
	model::Data::parameter_const_iterator p(mdl.parameter().begin());
	for(; p < mdl.parameter().end(); p++) {
		Create<Parameter>(*p);
	}
}

}

