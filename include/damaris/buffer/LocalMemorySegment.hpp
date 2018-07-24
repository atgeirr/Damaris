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

#ifndef __DAMARIS_LOCALSEGMENT_H
#define __DAMARIS_LOCALSEGMENT_H

#include <string>
#include <limits.h>
#include <stdlib.h>

#include "damaris/util/Deleter.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/buffer/Buffer.hpp"

namespace damaris {

/*
 * Contains the definition of a local memory segment in which
 * we can store chunks of variables. 
 */

class LocalMemorySegment : public Buffer {
	
	friend class Deleter<LocalMemorySegment>;
	
	/**
	 * Constructor. This function is private, creating a LocalMemorySegment
	 * object should be done through the New operator.
	 * 
	 */
	LocalMemorySegment() {} 
	
	/**
	 * Destructor.
	 */
	virtual ~LocalMemorySegment() {}

public:
	/**
	 * Returns a LocalMemorySegment implementation corresponding to the 
	 * described model.
	 * 
	 * \param[in] model : base model from configuration file.
	 */
    static std::shared_ptr<LocalMemorySegment> Create(
				const model::Buffer& model) {
        return std::shared_ptr<LocalMemorySegment>(new LocalMemorySegment(),
					Deleter<LocalMemorySegment>());
	}

public:
	/**
	 * Gets an absolute address from a relative Handle.
	 * 
	 * \param[in] h : Handle to translate into an absolute address.
	 */
	virtual ptr GetAddressFromHandle(const Handle& h) const {
		return NULL;
	}

	/**
	 * Gets a relative Handle from an absolute pointer.
	 *
	 * \param[in] p : pointer to translate into a Handle.
	 */
	virtual Handle GetHandleFromAddress(const ptr p) const {
		return Handle();
	}

	/**
	 * Allocates size bytes inside the shared memory segment
	 * and returns it as a DataSpace with this buffer as owner.
	 * If failed, the DataSpace will have a size of 0.
	 *
	 * \param[in] size : size to allocate.
	 */
	virtual DataSpace<Buffer> Allocate(const size_t& size) {
		Buffer::ptr t = malloc(size);
		if(t != NULL) {
			DataSpace<Buffer> ds(SHARED_FROM_THIS(),t,size);
			return ds;
		}
		return DataSpace<Buffer>();
	}

	/**
	 * Allocates size bytes inside the shared memory segment.
	 * The allocated memory is aligned on a multiple of the system's
	 * page size.
	 *
	 * \param[in] size : size to allocate.
	 */
	virtual DataSpace<Buffer> AllocateAligned(const size_t& size) {
		return Allocate(size);
	}

protected:
	/**
	 * Deallocate an allocated region.
	 * 
	 * \param[in] addr : pointer to the region that must be deallocated.
	 */
	virtual void Deallocate(ptr addr) {
		free(addr);
	}

public:
	/**
	 * Gets the amount of free memory left.
	 */
	virtual size_t GetFreeMemory() const {
		return SSIZE_MAX;
	}

	/**
	 * Waits until enough free memory is available. This function does not 
	 * reserve the memory and thus does not ensure that the next call to 
	 * allocate will work. Return false if the size will never be satisfied,
	 * true otherwise.
	 *
	 * \param[in] size : size to wait for.
	 */
	virtual bool WaitAvailable(const size_t& size) {
		return true;
	}

	/**
	 * Returns true if and only if the adress is in the shared memory 
	 * segment.
	 *
	 * \param[in] p : pointer to test.
	 */
	virtual bool PointerBelongsToSegment(const ptr p) const {
		return true;
	}
};

}


#endif
