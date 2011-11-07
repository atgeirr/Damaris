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
 * \file PyChunk.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "scripts/PyChunk.hpp"
#include <numpy/arrayobject.h>

namespace Damaris {

namespace bp = boost::python;
	
PyChunk::PyChunk()
{
	throw(bp::error_already_set());
}

PyChunk::PyChunk(Chunk* c)
{
	inner = c;
	if(inner == NULL) throw(bp::error_already_set());
}

int PyChunk::source() const 
{
	return inner->getSource();
}

int PyChunk::iteration() const 
{
	return inner->getIteration();
}

const std::string& PyChunk::type() const
{
	return Types::getStringFromType(inner->getType());
}

bp::list PyChunk::lower_bounds() const
{
	bp::list result;
	for(unsigned int i = 0; i < inner->getDimensions(); i++) {
		result.append(inner->getStartIndex((int)i));
	}
	return result;
}

bp::list PyChunk::upper_bounds() const
{
	bp::list result;
	for(unsigned int i = 0; i < inner->getDimensions(); i++) {
		result.append(inner->getEndIndex((int)i));
	}
	return result;
}

}

