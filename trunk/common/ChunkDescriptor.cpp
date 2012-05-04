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
 * \file ChunkHeader.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <string.h>
#include "common/ChunkDescriptor.hpp"

namespace Damaris {

ChunkDescriptor* ChunkDescriptor::New(unsigned int d, const int* lb, const int* ub)
{
	return new ChunkDescriptor(d,lb,ub);
}

ChunkDescriptor* ChunkDescriptor::New(const Layout& l)
{
	return new ChunkDescriptor(l);
}

void ChunkDescriptor::Delete(ChunkDescriptor* cd)
{
	if(cd != NULL) {
		delete cd;
	}
}

ChunkDescriptor::ChunkDescriptor(unsigned int d, const int* lb, const int* ub)
{
	dimensions = d;
	memset(lbounds,0,d*sizeof(int));
	memcpy(lbounds,lb,d*sizeof(int));
	memset(ubounds,0,d*sizeof(int));
	memcpy(ubounds,ub,d*sizeof(int));
}

ChunkDescriptor::ChunkDescriptor(const Layout& l)
{
    dimensions = l.getDimensions();
	for(int i = 0; i < (int)dimensions; i++ ) {
		lbounds[i] = 0;
		ubounds[i] = l.getExtentAlongDimension(i)-1;
	}
}

ChunkDescriptor::ChunkDescriptor(const ChunkDescriptor& ch)
{
	dimensions = ch.dimensions;
	memset(lbounds,0,dimensions*sizeof(int));
	memcpy(lbounds,ch.lbounds,dimensions*sizeof(int));
	memset(ubounds,0,dimensions*sizeof(int));
	memcpy(ubounds,ch.ubounds,dimensions*sizeof(int));
}

size_t ChunkDescriptor::getDataMemoryLength(const Model::Type &type) const
{
    size_t result = 1;
    result *= Types::basicTypeSize(type);
    for(unsigned int i = 0; i < dimensions; i++) {
        size_t d = (ubounds[i]-lbounds[i]+1);
        result *= d;
    }
    return result;
}

int ChunkDescriptor::getStartIndex(int i) const 
{
	if(0 <= i && i < (int)dimensions)
		return lbounds[i];
	else
		return 0;
}

int ChunkDescriptor::getEndIndex(int i) const
{
	if(0 <= i && i < (int)dimensions)
		return ubounds[i];
	else
		return 0;
}

bool ChunkDescriptor::within(const Layout& enclosing) const
{
    if(enclosing.isUnlimited()) return true;

    bool b = (enclosing.getDimensions() == getDimensions());
    if(b) {
        for(unsigned int i=0; i < getDimensions();i++) {
            b = b && (getStartIndex(i) >= 0);
            b = b && (getEndIndex(i) < (int)enclosing.getExtentAlongDimension(i));
        }
    }
    return b;
}

} // namespace Damaris

