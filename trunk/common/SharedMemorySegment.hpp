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
 * \file SharedMemoryBuffer.hpp
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * Contains the definition of a shared memory segment in which
 * we can store variables. This definition is abstract.
 */
#ifndef __DAMARIS_SHMEMSEGMENT_H
#define __DAMARIS_SHMEMSEGMENT_H

#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "common/SharedMemory.hpp"

namespace Damaris {

class SharedMemorySegment {
	private:
		std::string* name;	/*!< Name of the shared memory segment. */

		class POSIX_ShMem;
		class SYSV_ShMem;

		SharedMemorySegment();
	public:
		
		static SharedMemorySegment* create(posix_shmem_t shmem, const char* name, int64_t size);
		static SharedMemorySegment* create(sysv_shmem_t shmem, const char* name, int64_t size);

		static SharedMemorySegment* open(posix_shmem_t shmem, const char* name);
		static SharedMemorySegment* open(sysv_shmem_t shmem, const char* name);

		typedef void* ptr;

		virtual ptr getAddressFromHandle(handle_t h) = 0;
		virtual handle_t getHandleFromAddress(ptr p) = 0;
		virtual ptr allocate(size_t size) = 0;
		virtual void deallocate(void* addr) = 0;
		virtual size_t getFreeMemory() = 0;
};

using namespace boost::interprocess;

class SharedMemorySegment::POSIX_ShMem : public SharedMemorySegment {
	private:
		managed_shared_memory* impl;
		char* base_address;
	public:
		POSIX_ShMem(const char* name, int64_t size);
		POSIX_ShMem(const char* name);

		SharedMemorySegment::ptr getAddressFromHandle(handle_t h);
                handle_t getHandleFromAddress(SharedMemorySegment::ptr p);
                ptr allocate(size_t size);
                void deallocate(void* addr);
                size_t getFreeMemory();
};

class SharedMemorySegment::SYSV_ShMem : public SharedMemorySegment {
	public:
		SYSV_ShMem(const char* name, int64_t size);
		SYSV_ShMem(const char* name);

		SharedMemorySegment::ptr getAddressFromHandle(handle_t h);
                handle_t getHandleFromAddress(SharedMemorySegment::ptr p);
                ptr allocate(size_t size);
                void deallocate(void* addr);
                size_t getFreeMemory();
};

}
#endif
