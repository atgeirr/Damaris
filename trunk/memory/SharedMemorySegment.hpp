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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_SHMEMSEGMENT_H
#define __DAMARIS_SHMEMSEGMENT_H

#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_external_buffer.hpp>
#include <boost/interprocess/managed_xsi_shared_memory.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include "xml/Model.hpp"
#include "memory/SharedMemory.hpp"
#include "memory/Buffer.hpp"

namespace Damaris {

typedef boost::interprocess::basic_managed_external_buffer<
		char,
		boost::interprocess::rbtree_best_fit<boost::interprocess::mutex_family>,
		boost::interprocess::iset_index
	>
	managed_protected_external_buffer;
/*
 * Contains the definition of a shared memory segment in which
 * we can store chunks of variables. This definition is abstract.
 * Two internal classes are provided to implement SharedMemorySegment
 * based either on shm_open (posix) or shmget (xsi).
 */
using namespace boost::interprocess;

class SharedMemorySegment : public Buffer {
	protected:
		class CompositeShMem;
	
	protected:

		/**
		 * The size_manager_s structure is a header located at the beginning of a
		 * shared memory segment or a block, it holds information related to available
		 * size, as well as locks and interprocess conditions to handle concurrency.
		 */
		struct size_manager_s {
			interprocess_condition cond_size; /*!< Condition to block processes attempting 
												to allocate more than available size. */
			interprocess_mutex lock; /*!< To lock the size manager when accessing the size. */
			size_t size; /*!< Current available size (in bytes). Only updated when deallocating. */
			const size_t max; /*!< Maximum size (in bytes). */
			size_manager_s(size_t initSize): cond_size(), lock(), size(initSize), max(initSize) {};
		} *size_manager;
	
		mapped_region* region; /*!< pointer to the mapped_region object. */
		managed_protected_external_buffer* buffer; /*!< pointer to the managed buffer. */

		/**
		 * Constructor.
		 */
		SharedMemorySegment(mapped_region* r, managed_protected_external_buffer* b);

	public:
		/**
		 * Returns a SharedMemorySegment implementation corresponding
		 * to the described model.
		 * \param[in] model : base model from configuration file.
		 */
		static SharedMemorySegment* Create(Model::Buffer* model);

		/**
		 * Creates a POSIX-based shared memory segment with a given
		 * name and size.
		 */
		static SharedMemorySegment* Create(posix_shmem_t posix_shmem,
			const std::string& name, size_t size);

		/**
		 * Creates a SYSV-based shared memory segment with a given name
		 * and size.
		 */
		static SharedMemorySegment* Create(sysv_shmem_t sysv_shmem,
			const std::string& name, size_t size);

	private:
		/**
		 * Creates multiple blocks of shared memory based on a given modeli.
		 * This function is private and only used to create a Composite shared memory.
		 */
		static SharedMemorySegment* CreateMultiBlock(Model::Buffer* model);

	public:
		/**
		 * Opens a SharedMemorySegment implementation corresponding
		 * to the description in the model.
		 * \param[in] model : base model from configuration file.
		 */	
		static SharedMemorySegment* Open(Model::Buffer* model);

		/**
		 * Opens a POSIX-based shared memory segment.
		 * When size = 0, the system will try to query the actual
		 * shared memory size using fstat. If this function is not
		 * available on your platform, make sure to provide a size.
		 */
		static SharedMemorySegment* Open(posix_shmem_t posix_shmem,
			const std::string &name, size_t size = 0);

		/**
		 * Opens a SYSV-based shared memory segment.
		 * When size = 0, the system will try to query the actual
		 * shared memory size using fstat. If this function is not
		 * available on your platform, make sure to provide a size.
		 */
		static SharedMemorySegment* Open(sysv_shmem_t sysv_shmem,
			const std::string& name, size_t size = 0);

	private:
		/**
		 * Opens multiple blocks of shared memory.
		 */
		static SharedMemorySegment* OpenMultiBlock(Model::Buffer* model);

	public:
		/**
		 * Removes a SharedMemorySegment described in a model.
		 */
		static bool Remove(Model::Buffer* model);

		/**
		 * Removes a POSIX-based SharedMemorySegment.
		 */
		static bool Remove(posix_shmem_t posix_shmem,
			const std::string& name);
		
		/**
		 * Removes a SYSV-based SharedMemorySegment.
		 */
		static bool Remove(sysv_shmem_t sysv_shmem,
			const std::string& name);

	private:
		/**
		 * Removes multiple blocks of shared memory.
		 */
		static bool RemoveMultiBlock(const Model::Buffer* model);

	public:
		/**
		 * Gets an absolute address from a relative handle.
		 */
		virtual ptr getAddressFromHandle(handle_t h) const;

		/**
		 * Gets a relative handle from an absolute pointer.
		 */
		virtual handle_t getHandleFromAddress(ptr p) const;

		/**
		 * Allocates size bytes inside the shared memory segment.
		 */
		virtual ptr allocate(size_t size);

		/**
		 * Deallocate an allocated region.
		 */
		virtual void deallocate(void* addr);

		/**
		 * Gets the amount of free memory left.
		 */
		virtual size_t getFreeMemory() const;

		/**
		 * Waits until enough free memory is available.
		 * This function does not reserve the memory and thus
		 * does not ensure that the next call to allocate will work.
		 * Return false if the size will never be satisfied, true otherwise.
		 */
		virtual bool waitAvailable(size_t size);

		/**
		 * Returns true if and only if the adress is
		 * in the shared memory segment.
		 */
		virtual bool pointerBelongsToSegment(void* p) const;

		/**
		 * Destructor.
		 */
		virtual ~SharedMemorySegment();
};

using namespace boost::interprocess;

/**
 * The SharedMemorySegment::CompositeShMem class helps building
 * a shared memory segment with a size bigger than what is accepted
 * by the underlying operating system. It works by opening a set of
 * blocks instead of one single shared memory segment.
 * It is however not possible to allocate an object with a size
 * bigger than that of a block in this type of segment.
 */
class SharedMemorySegment::CompositeShMem : public SharedMemorySegment {
	private:
		std::vector<SharedMemorySegment*> *blocks; /*!< pointers to blocks of shared memory. */
		int nbseg; /*!< number of blocks. */

	public:
		/**
		 * Constructor. Takes the number of blocks and a pointer to a dynamically allocated
		 * vector of pointers to already initialized shared memory segments.
		 */
		CompositeShMem(int count, std::vector<SharedMemorySegment*>* segments);

		/**
		 * \see SharedMemorySegment::getAddressFromHandle
		 */
		virtual SharedMemorySegment::ptr getAddressFromHandle(handle_t h) const;

		/**
		 * \see SharedMemorySegment::getHandleFromAddress
		 */
		virtual handle_t getHandleFromAddress(SharedMemorySegment::ptr p) const;

		/**
		 * \see SharedMemorySegment::allocate
		 */
		virtual ptr allocate(size_t size);

		/**
		 * \see SharedMemorySegment::deallocate
		 */
		virtual void deallocate(void* addr);

		/**
		 * \see SharedMemorySegment::getFreeMemory();
		 */
		virtual size_t getFreeMemory() const;

		/**
		 * \see SharedMemorySegment::pointerBelongsToSegment
		 */
		virtual bool pointerBelongsToSegment(void* p) const;

		/**
		 * Destructor. Free the vector of blocks after deleting
		 * each block individually.
		 */
		virtual ~CompositeShMem(); 
};

}


#endif
