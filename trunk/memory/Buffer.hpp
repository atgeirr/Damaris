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
 * \file Buffer.hpp
 * \date Oct. 2012
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_BUFFER_H
#define __DAMARIS_BUFFER_H

#include "memory/SharedMemory.hpp"

namespace Damaris {

class Buffer {

	public:
		/**
		 * This typedef is just to prevent compilation error
		 * when defining pur virtual function that return void* pointers.
		 */
		typedef void* ptr;

		/**
		 * Gets an absolute address from a relative handle.
		 */
		virtual ptr GetAddressFromHandle(handle_t h) const = 0;

		/**
		 * Gets a relative handle from an absolute pointer.
		 */
		virtual handle_t GetHandleFromAddress(ptr p) const = 0;

		/**
		 * Allocates size bytes inside the shared memory segment.
		 */
		virtual ptr Allocate(size_t size) = 0;

		/**
		 * Deallocate an allocated region.
		 */
		virtual void Deallocate(void* addr) = 0;

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
		virtual bool WaitAvailable(size_t size) = 0;

		/**
		 * Returns true if and only if the adress is
		 * in the shared memory segment.
		 */
		virtual bool PointerBelongsToSegment(void* p) const = 0;

		/**
		 * Destructor.
		 */
		virtual ~Buffer() {}
};

}

#endif
