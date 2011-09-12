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
 * \file SharedMemorySegment.hpp
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#include "common/SharedMemorySegment.hpp"

namespace Damaris {

SharedMemorySegment::SharedMemorySegment()
{

}
	
SharedMemorySegment* SharedMemorySegment::create(posix_shmem_t posix_shmem, const char* name, int64_t size)
{
	return new SharedMemorySegment::POSIX_ShMem(name,size);
}

SharedMemorySegment* SharedMemorySegment::create(sysv_shmem_t sysv_shmem, const char* name, int64_t size)
{
	return new SharedMemorySegment::SYSV_ShMem(name,size);
}

SharedMemorySegment* SharedMemorySegment::open(posix_shmem_t posix_shmem, const char* name)
{
	return new SharedMemorySegment::POSIX_ShMem(name);
}

SharedMemorySegment* SharedMemorySegment::open(sysv_shmem_t sysv_shmem, const char* name)
{
	return new SharedMemorySegment::SYSV_ShMem(name);
}


SharedMemorySegment::POSIX_ShMem::POSIX_ShMem(const char* name, int64_t size)
{

}

SharedMemorySegment::POSIX_ShMem::POSIX_ShMem(const char* name)
{

}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const char* name, int64_t size)
{

}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const char* name)
{
	
}

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::getAddressFromHandle(handle_t h)
{
	return NULL;
}

handle_t SharedMemorySegment::POSIX_ShMem::getHandleFromAddress(SharedMemorySegment::ptr p)
{
	return 0;
}

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::allocate(size_t size)
{
	return NULL;
}

void SharedMemorySegment::POSIX_ShMem::deallocate(void* addr)
{

}

size_t SharedMemorySegment::POSIX_ShMem::getFreeMemory()
{
	return 0;
}

SharedMemorySegment::ptr SharedMemorySegment::SYSV_ShMem::getAddressFromHandle(handle_t h)
{
	return NULL;
}

handle_t SharedMemorySegment::SYSV_ShMem::getHandleFromAddress(SharedMemorySegment::ptr p) 
{               
	return 0;
}

SharedMemorySegment::ptr SharedMemorySegment::SYSV_ShMem::allocate(size_t size)
{
	return NULL;
}

void SharedMemorySegment::SYSV_ShMem::deallocate(void* addr) 
{               
	
}

size_t SharedMemorySegment::SYSV_ShMem::getFreeMemory()
{
	return 0;
}

}

