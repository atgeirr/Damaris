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
#ifndef __DAMARIS_SHMEMSEGMENT_H
#define __DAMARIS_SHMEMSEGMENT_H

#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_xsi_shared_memory.hpp>
#include "xml/Model.hpp"
#include "common/SharedMemory.hpp"

namespace Damaris {
/*
 * Contains the definition of a shared memory segment in which
 * we can store variables. This definition is abstract.
 * Two internal classes are provided to implement SharedMemorySegment
 * based either on shm_open (posix) or shmget (xsi).
 */
class SharedMemorySegment {
	private:
		class POSIX_ShMem;
		class SYSV_ShMem;

		/**
		 * Constructor.
		 */
		SharedMemorySegment();
	public:

		static SharedMemorySegment* create(Model::BufferModel* model);
	
		/**
		 * Creates a shared memory segment based on POSIX functions.
		 */	
		static SharedMemorySegment* create(posix_shmem_t shmem, 
				const char* name, int64_t size);

		/**
		 * Creates a shared memory segment based on XSI functions.
		 */
		static SharedMemorySegment* create(sysv_shmem_t shmem, 
				const char* name, int64_t size);

		static SharedMemorySegment* open(Model::BufferModel* model);
		/**
		 * Opens an existing shared memory segment based on POSIX functions.
		 */
		static SharedMemorySegment* open(posix_shmem_t shmem, const char* name);

		/**
		 * Opens an existing shared memory segment based in XSI functions.
		 */
		static SharedMemorySegment* open(sysv_shmem_t shmem, const char* name);

		static bool remove(Model::BufferModel* model);
		/**
		 * Removes an existing shared memory segment based on POSIX.
		 */
		static bool remove(posix_shmem_t shmem, const char* name);

		/**
		 * Removes an existing shared memory segment based on XSI.
		 */
		static bool remove(sysv_shmem_t shmem, const char* name);

		typedef void* ptr;

		/**
		 * Gets an absolute address from a relative handle.
		 */
		virtual ptr getAddressFromHandle(handle_t h) = 0;

		/**
		 * Gets a relative handle from an absolute pointer.
		 */
		virtual handle_t getHandleFromAddress(ptr p) = 0;

		/**
		 * Allocates size bytes inside the shared memory segment.
		 */
		virtual ptr allocate(size_t size) = 0;

		/**
		 * Deallocate an allocated region.
		 */
		virtual void deallocate(void* addr) = 0;

		/**
		 * Gets the amount of free memory left.
		 */
		virtual size_t getFreeMemory() = 0;
};

using namespace boost::interprocess;

class SharedMemorySegment::POSIX_ShMem : public SharedMemorySegment {
	private:
		managed_shared_memory* impl;
	public:
		POSIX_ShMem(const std::string &name, int64_t size);
		POSIX_ShMem(const std::string &name);

		SharedMemorySegment::ptr getAddressFromHandle(handle_t h);
                handle_t getHandleFromAddress(SharedMemorySegment::ptr p);
                ptr allocate(size_t size);
                void deallocate(void* addr);
                size_t getFreeMemory();
};

class SharedMemorySegment::SYSV_ShMem : public SharedMemorySegment {
	private:
		managed_xsi_shared_memory* impl;
		xsi_key key;
	public:
		SYSV_ShMem(const std::string &name, int64_t size);
		SYSV_ShMem(const std::string &name);

		SharedMemorySegment::ptr getAddressFromHandle(handle_t h);
                handle_t getHandleFromAddress(SharedMemorySegment::ptr p);
                ptr allocate(size_t size);
                void deallocate(void* addr);
                size_t getFreeMemory();
};

}
#endif
