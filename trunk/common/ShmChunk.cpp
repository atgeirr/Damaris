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
 * \file ShmChunk.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "common/ShmChunk.hpp"

namespace Damaris {

ShmChunk::ShmChunk(SharedMemorySegment* s, Types::basic_type_e t, 
		unsigned int d, std::vector<int> &si, std::vector<int> &ei)
: Chunk(t,d,si,ei)
{
	segment = s;
	header = NULL;
	buffer = NULL;

	size_t sizeOfHeader = size();
	size_t sizeOfData = getDataMemoryLength();
	size_t totalSize = sizeOfHeader + sizeOfData;

	header = static_cast<void*>(segment->allocate(totalSize));
	buffer = ((char*)header)+sizeOfHeader;

	toBuffer(header);
}

ShmChunk::ShmChunk(SharedMemorySegment* s, handle_t h)
{
	segment = s;
	header = segment->getAddressFromHandle(h);
	fromBuffer(header);
	buffer = ((char*)header)+size();
}

ShmChunk::~ShmChunk()
{
}

void* ShmChunk::data()
{
	return buffer;
}

bool ShmChunk::remove()
{
	if(buffer == NULL) return false;
	
	segment->deallocate(header);
	header = NULL;
	buffer = NULL;
	return true;
}

handle_t ShmChunk::getHandle()
{
	return segment->getHandleFromAddress(header);
}

void ShmChunk::setSource(int s)
{
	source = s;
	memcpy(header,&s,sizeof(s));
}

void ShmChunk::setIteration(int i)
{
	iteration = i;
	memcpy(((char*)header)+sizeof(source),&i,sizeof(i));
}

} // namespace Damaris
