/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

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

#include "damaris/util/Deleter.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/buffer/Buffer.hpp"

namespace damaris {

typedef boost::interprocess::basic_managed_external_buffer<
		char,
		boost::interprocess::rbtree_best_fit<
			boost::interprocess::mutex_family>,
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
	
	friend class Deleter<SharedMemorySegment>;
	
protected:
	class CompositeShMem;
	
protected:

	/**
	* The size_manager_s structure is a header located at the 
	* beginning of a shared memory segment or a block, it holds 
	* information related to available size, as well as locks and 
	* interprocess conditions to handle concurrency.
	*/
	struct size_manager_s {
		interprocess_condition cond_size; /*!< Condition to 
			block processes attempting to allocate more than 
			available size. */
		interprocess_mutex lock; /*!< To lock the size manager when 
					accessing the size. */
		size_t size; /*!< Current available size (in bytes). Only 
			updated when deallocating. */
		const size_t max; /*!< Maximum size (in bytes). */
		size_manager_s(size_t initSize): 
			cond_size(), lock(), 
			size(initSize), max(initSize) {};
	} *size_manager_;
	
	shared_ptr<mapped_region> region_; /*!< mapped_region object. */
	shared_ptr<managed_protected_external_buffer> buffer_; 
		/*!< pointer to the managed buffer. */

	/**
	 * Constructor. This function is private, creating a SharedMemorySegment
	 * object should be done through the New operator.
	 * 
	 * \param[in] r : pointer to a mapped_region (boost object).
	 * \param[in] b : pointer to the managed external buffer (boost object).
	 */
	SharedMemorySegment(const shared_ptr<mapped_region>& r, 
		const shared_ptr<managed_protected_external_buffer>& b);
	
	/**
	 * Destructor.
	 */
	virtual ~SharedMemorySegment() {}

public:
	/**
	 * Returns a SharedMemorySegment implementation corresponding to the 
	 * described model.
	 * 
	 * \param[in] model : base model from configuration file.
	 */
	static shared_ptr<SharedMemorySegment> Create(
				const model::Buffer& model);

	/**
	 * Creates a POSIX-based shared memory segment with a given
	 * name and size.
	 * 
	 * \param[in] posix_shmem : dummy argument to specify POSIX shm.
	 * \param[in] name : name of the shared memory segment.
	 * \param[in] size : size of the shared memory segment.
	 */
	static shared_ptr<SharedMemorySegment> Create(posix_shmem_t posix_shmem,
		const std::string& name, size_t size);

	/**
	 * Creates a SYSV-based shared memory segment with a given name
	 * and size.
	 * 
	 * \param[in] sysv_shmem : dummy argument to specify SYSV shm.
	 * \param[in] name : name of the shared memory segment.
	 * \param[in] size : size of the shared memory segment.
	 */
	static shared_ptr<SharedMemorySegment> Create(sysv_shmem_t sysv_shmem,
		const std::string& name, size_t size);

private:
	/**
	 * Creates multiple blocks of shared memory based on a given modeli.
	 * This function is private and only used to create a Composite shared 
	 * memory.
	 * 
	 * \param[in] model : model from configuration file.
	 */
	static shared_ptr<SharedMemorySegment> CreateMultiBlock(
			const model::Buffer& model);

public:
	/**
	 * Opens a SharedMemorySegment implementation corresponding
	 * to the description in the model.
	 * 
	 * \param[in] model : base model from configuration file.
	 */	
	static shared_ptr<SharedMemorySegment> Open(const model::Buffer& model);

	/**
	 * Opens a POSIX-based shared memory segment.
	 * When size = 0, the system will try to query the actual
	 * shared memory size using fstat. If this function is not
	 * available on your platform, make sure to provide a size.
	 *
	 * \param[in] posix_shmem : dummy argument to force POSIX shm.
	 * \param[in] name : name of the shared memory segment.
	 * \param[in] size : size of the shared memory segment.
	 */
	static shared_ptr<SharedMemorySegment> Open(posix_shmem_t posix_shmem,
		const std::string &name, size_t size = 0);

	/**
	 * Opens a SYSV-based shared memory segment.
	 * When size = 0, the system will try to query the actual
	 * shared memory size using fstat. If this function is not
	 * available on your platform, make sure to provide a size.
	 *
	 * \param[in] sysv_shmem : dummy argument to force SYSV shm.
	 * \param[in] name : name of the shared memory segment.
	 * \param[in] size : size of the shared memory segment.
	 */
	static shared_ptr<SharedMemorySegment> Open(sysv_shmem_t sysv_shmem,
		const std::string& name, size_t size = 0);

private:
	/**
	 * Opens multiple blocks of shared memory.
	 * 
	 * \param[in] model : base model from configuration file.
	 */
	static shared_ptr<SharedMemorySegment> OpenMultiBlock(
			const model::Buffer& model);

public:
	/**
	 * Removes a SharedMemorySegment described in a model.
	 *
	 * \param[in] model : base model from configuration file.
	 */
	static bool Remove(const model::Buffer& model);

	/**
	 * Removes a POSIX-based SharedMemorySegment.
	 * 
	 * \param[in] posix_shmem : dummy argument to force POSIX shm.
	 * \param[in] name : name of the shared memory segment to remove.
	 */
	static bool Remove(posix_shmem_t posix_shmem, const std::string& name);
		
	/**
	 * Removes a SYSV-based SharedMemorySegment.
	 * 
	 * \param[in] sysv_shmem : dummy argument to force SYSV shm.
	 * \param[in] name : name of the shared memory segment to remove.
	 */
	static bool Remove(sysv_shmem_t sysv_shmem, const std::string& name);

private:
	/**
	 * Removes multiple blocks of shared memory.
	 * 
	 * \param[in] model : model from configuration file.
	 */
	static bool RemoveMultiBlock(const model::Buffer& model);

public:
	/**
	 * Gets an absolute address from a relative Handle.
	 * 
	 * \param[in] h : Handle to translate into an absolute address.
	 */
	virtual ptr GetAddressFromHandle(const Handle& h) const;

	/**
	 * Gets a relative Handle from an absolute pointer.
	 *
	 * \param[in] p : pointer to translate into a Handle.
	 */
	virtual Handle GetHandleFromAddress(const ptr p) const;

	/**
	 * Allocates size bytes inside the shared memory segment
	 * and returns it as a DataSpace with this buffer as owner.
	 * If failed, the DataSpace will have a size of 0.
	 *
	 * \param[in] size : size to allocate.
	 */
	virtual DataSpace<Buffer> Allocate(const size_t& size);

	/**
	 * Allocates size bytes inside the shared memory segment.
	 * The allocated memory is aligned on a multiple of the system's
	 * page size.
	 *
	 * \param[in] size : size to allocate.
	 */
	virtual DataSpace<Buffer> AllocateAligned(const size_t& size);

protected:
	/**
	 * Deallocate an allocated region.
	 * 
	 * \param[in] addr : pointer to the region that must be deallocated.
	 */
	virtual void Deallocate(ptr addr);

public:
	/**
	 * Gets the amount of free memory left.
	 */
	virtual size_t GetFreeMemory() const;

	/**
	 * Waits until enough free memory is available. This function does not 
	 * reserve the memory and thus does not ensure that the next call to 
	 * allocate will work. Return false if the size will never be satisfied,
	 * true otherwise.
	 *
	 * \param[in] size : size to wait for.
	 */
	virtual bool WaitAvailable(const size_t& size);

	/**
	 * Returns true if and only if the adress is in the shared memory 
	 * segment.
	 *
	 * \param[in] p : pointer to test.
	 */
	virtual bool PointerBelongsToSegment(const ptr p) const;
};

/**
 * The SharedMemorySegment::CompositeShMem class helps building a shared memory 
 * segment with a size bigger than what is accepted by the underlying operating 
 * system. It works by opening a set of blocks instead of one single shared 
 * memory segment. It is however not possible to allocate an object with a size
 * bigger than that of a block in this type of segment.
 */
class SharedMemorySegment::CompositeShMem : public SharedMemorySegment {
	
	friend class Deleter<CompositeShMem>;
	
private:
	std::vector<shared_ptr<SharedMemorySegment> > blocks_; 
		/*!< pointers to blocks of shared memory. */
	int nbseg_; /*!< number of blocks. */

public:
	/**
	 * Constructor. Takes the number of blocks and a pointer to a 
	 * dynamically allocated vector of pointers to already initialized 
	 * shared memory segments.
	 */
	CompositeShMem(int count, 
		const std::vector<shared_ptr<SharedMemorySegment> >& segments);

	/**
	 * \see Buffer::GetAddressFromHandle
	 */
	virtual Buffer::ptr GetAddressFromHandle(const Handle& h) const;

	/**
	 * \see Buffer::GetHandleFromAddress
	 */
	virtual Handle GetHandleFromAddress(const Buffer::ptr p) const;

	/**
	 * \see Buffer::Allocate
	 */
	virtual DataSpace<Buffer> Allocate(const size_t& size);
	
	/**
	 * \see Buffer::AllocateAligned
	 */
	virtual DataSpace<Buffer> AllocateAligned(const size_t& size);

protected:
	/**
	 * \see Buffer::Deallocate
	 */
	virtual void Deallocate(Buffer::ptr addr);

public:
	/**
	 * \see Buffer::GetFreeMemory();
	 */
	virtual size_t GetFreeMemory() const;

	/**
	 * \see Buffer::PointerBelongsToSegment
	 */
	virtual bool PointerBelongsToSegment(Buffer::ptr p) const;

	/**
	 * Destructor. Free the vector of blocks after deleting
	 * each block individually.
	 */
	virtual ~CompositeShMem() {}
};

}


#endif
