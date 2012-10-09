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
 * \file ChunkImpl.cpp
 * \date Oct. 2012
 * \author Matthieu Dorier
 * \version 0.7
 */
#include "data/ChunkImpl.hpp"

namespace Damaris {

ChunkImpl::ChunkImpl(Buffer* b, ChunkHeader* ch) 
{
	isOwner = false;
	buffer = b;
	header = ch;
	addr = ((char*)header)+sizeof(ChunkHeader);
}

ChunkImpl::ChunkImpl(Buffer* b, handle_t h)
{
	isOwner = false;
	buffer = b;
	header = (ChunkHeader*)buffer->getAddressFromHandle(h);
	addr = ((char*)header)+sizeof(ChunkHeader);
}

ChunkImpl::~ChunkImpl()
{
	if(isOwner) {
		buffer->deallocate(header);
	}
}

void* ChunkImpl::data()
{
	return addr;
}

size_t ChunkImpl::MemCopy(const void* src)
{
	size_t size = header->getDataMemoryLength(getType());
	memcpy(addr,src,size);
	return size;
}

handle_t ChunkImpl::getHandle()
{
	return buffer->getHandleFromAddress(header);
}

} // namespace Damaris
