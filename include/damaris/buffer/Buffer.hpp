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
#ifndef __DAMARIS_BUFFER_H
#define __DAMARIS_BUFFER_H

#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/buffer/DataSpace.hpp"
#include "damaris/buffer/SharedMemory.hpp"
#include "damaris/buffer/Mode.hpp"

namespace damaris {


/**
 * The Buffer class is an abstract class that gives direct access to a (usually
 * shared) segment of memory. It is one of the few classes in Damaris that 
 * works with raw pointers instead of std::shared_ptr, because it is supposed to
 * allocate data that stays in shared memory until another process removes it,
 * something that is prevented by std::shared_ptr.
 */
class Buffer : public ENABLE_SHARED_FROM_THIS(Buffer)
{
	friend class Deleter<Buffer>;
	friend class Manager<Buffer>;
	friend class DataSpace<Buffer>;

	protected:
	
	int id_; /*!< Id of the buffer. */
	std::string name_; /*!< name of the buffer. */

	/**
	 * Destructor.
	 */
	virtual ~Buffer() {}
	
	public:
	/**
	 * This typedef is just to prevent compilation error when defining pur 
	 * virtual function that return void* pointers.
	 */
	typedef void* ptr;

	/**
	 * Access to the ID of the buffer.
	 */
	int GetID() const {
		return id_;
	}
	
	/**
	 * Access the name of the buffer.
	 */
	const std::string& GetName() const {
		return name_;
	}

	/**
	 * Gets an absolute address from a relative handle.
	 */
	virtual ptr GetAddressFromHandle(const Handle& h) const = 0;

	/**
	 * Gets a relative handle from an absolute pointer.
	 */
	virtual Handle GetHandleFromAddress(const ptr p) const = 0;

	/**
	 * Allocates size bytes inside the shared memory segment.
	 */
	virtual DataSpace<Buffer> Allocate(const size_t& size) = 0;
	
	/**
	 * Allocates size bytes inside the shared memory segment.
	 * The result is aligned on a multiple of the system's page size.
	 */
	virtual DataSpace<Buffer> AllocateAligned(const size_t& size) = 0;

protected:
	/**
	 * Deallocate an allocated region.
	 */
	virtual void Deallocate(ptr addr) = 0;

public:
	/**
	 * Gets the amount of free memory left.
	 */
	virtual size_t GetFreeMemory() const = 0;

	/**
	 * Waits until enough free memory is available.
	 * This function does not reserve the memory and thus
	 * does not ensure that the next call to allocate will work.
	 * Return false if the size will never be satisfied, true otherwise.
	 */
	virtual bool WaitAvailable(const size_t& size) = 0;

	/**
	 * Returns true if and only if the adress is 
	 * in the shared memory segment.
	 */
	virtual bool PointerBelongsToSegment(const ptr p) const = 0;
	
	/**
	 * Change the memory access authorizations for a portion of
	 * memory. The address should be aligned on page size, size should
	 * be a multiple of the page size and the protection should be one of 
	 * the constants defined in Mode.hpp.
	 */
	virtual bool ChangeMode(ptr p, size_t size, int protection) {
		int pagesize = sysconf(_SC_PAGE_SIZE);
		if(pagesize == -1) return false;
		
		if(((int64_t)p) % pagesize != 0) return false;
		if(size % pagesize != 0) size -= (size % pagesize);
		
		if(mprotect(p, size, protection) == -1) return false;
		return true;
	}
	
};

}

#endif
