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
 * \file PyLayout.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "scripts/PyLayout.hpp"

namespace Damaris {

namespace bp = boost::python;

PyLayout::PyLayout()
{
	throw(bp::error_already_set());
}
	
PyLayout::PyLayout(Layout* l)
{
	inner = l;
	if(inner == l) throw(bp::error_already_set());
}

std::string PyLayout::name()
{
	return inner->getName();
}

std::string PyLayout::type()
{
	return "";//TODO
}
		
std::vector<int> PyLayout::extents()
{
	std::vector<int> result(inner->getDimensions());
	for(unsigned int i=0; i < inner->getDimensions(); i++) {
		result[i] = inner->getExtentAlongDimension(i);
	}
}

}
