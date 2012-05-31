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
#include "data/ShmChunk.hpp"

namespace Damaris {

ShmChunk::ShmChunk(SharedMemorySegment* s, ChunkHeader* ch) 
{
	isOwner = false;
	segment = s;
	header = ch;
	buffer = ((char*)header)+sizeof(ChunkHeader);
}

ShmChunk::ShmChunk(SharedMemorySegment* s, handle_t h)
{
	isOwner = false;
	segment = s;
	header = (ChunkHeader*)segment->getAddressFromHandle(h);
	buffer = ((char*)header)+sizeof(ChunkHeader);
}

ShmChunk::~ShmChunk()
{
	if(isOwner) {
		segment->deallocate(header);
	}
}

void* ShmChunk::data()
{
	return buffer;
}

handle_t ShmChunk::getHandle()
{
	return segment->getHandleFromAddress(header);
}

} // namespace Damaris
