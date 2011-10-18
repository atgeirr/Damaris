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
 * \file Chunk.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * This file defines the Chunk object.
 */
#include <string.h> 
#include "common/Chunk.hpp"

namespace Damaris {

/**

 */	

Chunk::Chunk(Types::basic_type_e t, unsigned int d, std::vector<int> &si, std::vector<int> &ei)
{
	source = -1;
	iteration = -1;

	type = t;
	dimensions = d;

	startIndices = si;
	endIndices = ei;
}

Chunk::~Chunk()
{

}

int Chunk::getSource()
{
	return source;
}

void Chunk::setSource(int s)
{
	source = s;
}

int Chunk::getIteration()
{
	return iteration;
}

void Chunk::setIteration(int i)
{
	iteration = i;
}

unsigned int Chunk::getDimensions()
{
	return dimensions;
}

Types::basic_type_e Chunk::getType()
{
	return type;
}

size_t Chunk::getRequiredMemoryLength()
{
	size_t result = 1;
	result *= Types::basicTypeSize(type);
	for(unsigned int i = 0; i < dimensions; i++) {
		size_t d = (endIndices[i]-startIndices[i]+1);
		result *= d;
	}
	return result;
}

bool Chunk::within(Layout* enclosing)
{
	bool b = (enclosing->getDimensions() == dimensions);
	b = b && (enclosing->getType() == type);
	if(b) {
		for(unsigned int i=0; i < dimensions;i++) {
			b = b && (startIndices[i] >= 0);
			b = b && (endIndices[i] < enclosing->getExtentAlongDimension(i));
		}
	}
	return b;
}

bool Chunk::within(Chunk* enclosing)
{
	bool b = (enclosing->getDimensions() == dimensions);
	b = b && (enclosing->getType() == type);
	if(b) {
		for(unsigned int i=0; i < dimensions; i++) {
			b = b && (startIndices[i] >= enclosing->getStartIndex(i));
			b = b && (endIndices[i] <= enclosing->getEndIndex(i));
		}
	}
	return b;
}

size_t Chunk::size()
{
	size_t s = 0;
	s += sizeof(source);
	s += sizeof(iteration);
	s += sizeof(type);
	s += sizeof(dimensions);
	s += dimensions*2*sizeof(int);
	return s;
}

void Chunk::toBuffer(void* buffer)
{
	char* dst = (char*)buffer;
	int offset = 0;

	memcpy(dst+offset,&source,sizeof(source));
		offset += sizeof(source);
	memcpy(dst+offset,&iteration,sizeof(iteration));
		offset += sizeof(iteration);
	memcpy(dst+offset,&type,sizeof(type));
		offset += sizeof(type);
	memcpy(dst+offset,&dimensions,sizeof(dimensions));
		offset += sizeof(dimensions);
	if(dimensions != 0) {
		memcpy(dst+offset,&(startIndices[0]),dimensions*sizeof(int));
			offset += dimensions*sizeof(int);
		memcpy(dst+offset,&(endIndices[0]),dimensions*sizeof(int));
			offset += dimensions*sizeof(int);
	}
}

void Chunk::fromBuffer(const void* buffer)
{
	const char* src = (const char*)buffer;
	int offset = 0;

	memcpy(&source,src+offset,sizeof(source));
		offset += sizeof(source);
	memcpy(&iteration,src+offset,sizeof(iteration));
		offset += sizeof(iteration);
	memcpy(&type,src+offset,sizeof(type));
		offset += sizeof(type);
	memcpy(&dimensions,src+offset,sizeof(dimensions));
		offset += sizeof(dimensions);
	if(dimensions != 0) {
		if(startIndices.size() != dimensions) {
			startIndices.resize(dimensions);
		}
		memcpy(&(startIndices[0]),src+offset,dimensions*sizeof(int));
			offset += dimensions*sizeof(int);
		if(endIndices.size() != dimensions) {
			endIndices.resize(dimensions);
		}
		memcpy(&(endIndices[0]),src+offset,dimensions*sizeof(int));
			offset += dimensions*sizeof(int);
	}
}

} // namespace Damaris
