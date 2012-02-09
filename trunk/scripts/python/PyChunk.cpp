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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "scripts/python/PyChunk.hpp"
#include "common/Debug.hpp"
#define PY_ARRAY_UNIQUE_SYMBOL damaris_ARRAY_API
#define NO_IMPORT_ARRAY
#include <numpy/arrayobject.h>
#include "scripts/python/PyTypes.hpp"

namespace Damaris {

namespace Python {

namespace bp = boost::python;
	
PyChunk::PyChunk()
{
	ERROR("Trying to initialize a PyChunk");
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

bp::object PyChunk::data() const
{
	int nd = inner->getDimensions();
	npy_intp *dims = new npy_intp[nd];
	for(int i = 0; i < nd; i++) {
		dims[i] = inner->getEndIndex(i) - inner->getStartIndex(i) + 1;
	}
	int typenum = PyTypes::getPyTypeFromDamarisType(inner->getType());

	if(typenum == -1) {
		throw(bp::error_already_set());
	}

	void *data = inner->data();
	if(data == NULL) return bp::object();

	PyObject* arr = PyArray_SimpleNewFromData(nd, dims, typenum, data);
	delete[] dims;
	return bp::object(bp::handle<>(arr));
}

}
}
