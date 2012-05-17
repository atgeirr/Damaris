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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "scripts/python/PyVariable.hpp"
#include "scripts/python/PyChunk.hpp"
#include "core/Debug.hpp"

namespace Damaris {

namespace Python {

namespace bp = boost::python;

PyVariable::PyVariable() 
{
	INFO("Trying to create a PyVariable with the wrong constructor");
	throw(bp::error_already_set());
}

PyVariable::PyVariable(Variable* v) 
{
	inner = v;
}

bp::list PyVariable::select(const bp::dict &args) const
{
	bp::list result;
	if(inner == NULL) return result;
	// gets the iteration number
	if(args.has_key("iteration")) {
		int iteration = bp::extract<int>(args["iteration"]);
		if(args.has_key("source")) {
			int source = bp::extract<int>(args["source"]);
			ChunkIndex::iterator it;
			ChunkIndex::iterator end;
			it = inner->getChunks(source,iteration,end);
			while(it != end) {
				result.append(PyChunk(it->get()));
				it++;
			}
		} else {
			ChunkIndexByIteration::iterator it;
			ChunkIndexByIteration::iterator end;
			it = inner->getChunksByIteration(iteration,end);
			while(it != end) {
				result.append(PyChunk(it->get()));
				it++;
			}
		}
	} else {
		if(args.has_key("source")) {
			int source = bp::extract<int>(args["source"]);
			ChunkIndexBySource::iterator it;
			ChunkIndexBySource::iterator end;
			it = inner->getChunksBySource(source,end);
			while(it != end) {
				result.append(PyChunk(it->get()));
				it++;
			}
		} else {
			ChunkIndex::iterator it;
			ChunkIndex::iterator end;
			it = inner->getChunks(end);
			while(it != end) {
				result.append(PyChunk(it->get()));
				it++;
			}
		}
	}
	return result;
}

bp::list PyVariable::chunks() const 
{
	return select(bp::dict());
}

std::string PyVariable::name() const
{
	int last_slash = inner->getName().find_last_of('/');
	if(last_slash == 0) last_slash = -1;
	return inner->getName().substr(last_slash+1);
}

std::string PyVariable::description() const
{
	return inner->getDescription();
}

std::string PyVariable::unit() const
{
	return inner->getUnit();
}

const std::string& PyVariable::fullname() const
{
	return inner->getName();
}

PyLayout PyVariable::layout() const
{
	return PyLayout(inner->getLayout());
}

bool PyVariable::remove(PyChunk& pc)
{
	WARN("This function is not implemented");
	return false;
}

void PyVariable::clear()
{
	inner->clear();
}

}
}
