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
 * \file PyVariable.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "scripts/PyVariable.hpp"
#include "scripts/PyChunk.hpp"

namespace Damaris {

namespace bp = boost::python;

PyVariable::PyVariable() 
{
	throw(bp::error_already_set());
}

PyVariable::PyVariable(Variable* v) 
{
	inner = v;
}

bp::list PyVariable::chunks(const bp::dict &args)
{
	bp::list result;
	if(inner == NULL) return result;
	ChunkIndex::iterator it;
	ChunkIndex::iterator end;
	it = inner->getChunks(end);
	while(it != end) {
		result.append(PyChunk(it->get()));
	}
	return result;
}

}
