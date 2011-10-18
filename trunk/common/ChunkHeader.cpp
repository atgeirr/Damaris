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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * This file defines the Chunk object.
 */

#include <string.h>
#include "common/ChunkHeader.hpp"

namespace Damaris {
	
size_t ChunkHeader::size() 
{
	return sizeof(Types::basic_type_e)+sizeof(unsigned int)+dimensions*2*sizeof(int);
}

void ChunkHeader::toBuffer(void* dest) const 
{
	int offset = 0;
	char* dst = (char*)dest;
	memcpy(dst,&type,sizeof(Types::basic_type_e));
	offset += sizeof(Types::basic_type_e);
	memcpy(dst+offset,&dimensions,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(dst+offset,startIndex,dimensions*sizeof(int));
	offset += sizeof(int)*dimensions;
	memcpy(dst+offset,endIndex,dimensions*sizeof(int));
}

ChunkHeader ChunkHeader::fromBuffer(void* src)
{
	char* ptr = (char*)src;
	Types::basic_type_e type = *((Types::basic_type_e*)(ptr));
	ptr += sizeof(Types::basic_type_e);
	unsigned int dimensions = *((unsigned int*)(ptr));
	int* si = (int*)(ptr+sizeof(unsigned int));
	int* ei = si + dimensions;
	ChunkHeader chunk(type,dimensions,si,ei);
	return chunk;
}

size_t  ChunkHeader::getRequiredMemoryLength() const
{
	size_t result = 1;
	result *= basicTypeSize(type);
	for(unsigned int i = 0; i < dimensions; i++) {
		size_t d = (endIndex[i]-startIndex[i]+1);
		result *= d;
	}
	return result;
}
	
int ChunkHeader::getExtentAlongDimension(unsigned int dim) const
{
	return (endIndex[dim]-startIndex[dim]+1);
}
		
ChunkHeader::ChunkHeader(Types::basic_type_e t, unsigned int d)
{
	type = t;
	dimensions = d;
	startIndex = (int*)malloc(d*sizeof(int));
	endIndex = (int*)malloc(d*sizeof(int));
}

ChunkHeader::ChunkHeader(Types::basic_type_e t, unsigned int d, int* sIndex, int* eIndex)
{
	type = t;
	dimensions = d;
	startIndex = (int*)malloc(d*sizeof(int));
	endIndex = (int*)malloc(d*sizeof(int));
	for(unsigned int i=0; i < d; i++) {
		startIndex[i] = sIndex[i];
		endIndex[i] = eIndex[i];
	}
}

ChunkHeader::ChunkHeader(const ChunkHeader& h)
{
	type = h.type;
	dimensions = h.dimensions;
	startIndex = (int*)malloc(dimensions*sizeof(int));
	endIndex = (int*)malloc(dimensions*sizeof(int));
	for(unsigned int i=0; i < dimensions; i++){
		startIndex[i] = h.startIndex[i];
		endIndex[i] = h.endIndex[i];
	}
}
/*
ChunkHeader::ChunkHeader(ChunkHeader h)
{
        type = h.type;
        dimensions = h.dimensions;
        startIndex = (int*)malloc(dimensions*sizeof(int));
        endIndex = (int*)malloc(dimensions*sizeof(int));
        for(unsigned int i=0; i < dimensions; i++){
                startIndex[i] = h.startIndex[i];
                endIndex[i] = h.endIndex[i];
        }
}*/

ChunkHeader::~ChunkHeader()
{
	if(startIndex) free(startIndex);
	if(endIndex) free(endIndex);
}

}
