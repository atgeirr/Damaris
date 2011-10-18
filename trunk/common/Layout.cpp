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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */

#include <cstring>
#include "common/Debug.hpp"
#include "common/Layout.hpp"
#include "common/Chunk.hpp"

namespace Damaris {
	
	Layout::Layout(Types::basic_type_e t, unsigned int d, std::vector<int> &ex)
	{
		type = t;
		dimensions = d;
		if(ex.size() != d) {
			ERROR("In layout initialization: extents size and dimensions do not match");
		}
		extents = ex;
		/*
		if(dimensions > 0){
			startIndex = new int64_t[dimensions];
			endIndex = new int64_t[dimensions];
			for(int i = 0; (i < dimensions) && (2*i+1 < (int)extents.size()); i++)
			{
				startIndex[i] = extents[2*i];
				endIndex[i] = extents[2*i+1];
			}
		} else {
			startIndex = NULL;
			endIndex = NULL;
		}
		*/
	}
	/*
	Layout::Layout(Types::basic_type_e t, int32_t d)
	{
		type = t;
		dimensions = d;
		if(d != 0) {
			ERROR("Error in layout initialization");
		}
		startIndex = NULL;
		endIndex = NULL;
	}
	*/
	Layout::~Layout()
	{
	//	if(startIndex) delete startIndex;
	//	if(endIndex) delete endIndex;
	}
	/*
	size_t  Layout::getRequiredMemoryLength() const
	{
		size_t result = 1;
		result *= basicTypeSize(type);
		for(unsigned int i = 0; i < dimensions; i++) {
			size_t d = extents[i]; //(endIndex[i]-startIndex[i]+1);
			result *= d;
		}
		return result;
	}
	
	Types::basic_type_e Layout::getType() const
	{
		return type;
	}
	
	unsigned int Layout::getDimensions() const
	{
		return dimensions;
	}

	int64_t Layout::getStartIndex(int dim) const
	{
		return startIndex[dim];
	}
	
	int64_t Layout::getEndIndex(int dim) const
	{
		return endIndex[dim];
	}
*/
	int Layout::getExtentAlongDimension(unsigned int dim) const
	{
		if(dim < dimensions)
			return extents[dim]; //return (endIndex[dim]-startIndex[dim]+1);
		else
			return 0;
	}

/*	Chunk toChunk() const
	{
		std::vector<int> si;
		std::vector<int> ei;
		for(std::vector<int>::const_iterator it = extents.begin(); it != extents.end(); it++) {
			si.push_back((*it) - 1);
			ei.push_back(0);
		}
		return Chunk(type,dimensions,si,ei);
	}
*/
}
