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
 * \file Layout.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#include <cstring>
#include "common/Debug.hpp"
#include "common/Layout.hpp"

namespace Damaris {
	
	Layout::Layout(Types::basic_type_e t, unsigned int d, std::vector<int> &ex)
	{
		type = t;
		dimensions = d;
		if(ex.size() != d) {
			ERROR("In layout initialization: extents size and dimensions do not match");
		}
		extents = ex;
	}
	
	Layout::~Layout()
	{
	}
	
	Types::basic_type_e Layout::getType() const
	{
		return type;
	}
	
	unsigned int Layout::getDimensions() const
	{
		return dimensions;
	}
	
	int Layout::getExtentAlongDimension(unsigned int dim) const
	{
		if(dim < dimensions)
			return extents[dim];
		else
			return 0;
	}
}
