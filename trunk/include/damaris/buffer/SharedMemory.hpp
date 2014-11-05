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

#ifndef __DAMARIS_SHARED_MEMORY_H
#define __DAMARIS_SHARED_MEMORY_H

#include <boost/interprocess/managed_external_buffer.hpp>

namespace damaris {

/**
 * POSIX shared memory support (based on shm_open).
 */
struct posix_shmem_t { };

/**
 * System-V shared memory support (based on shmget).
 */
struct sysv_shmem_t { };

static const struct posix_shmem_t posix_shmem = posix_shmem_t();
static const struct sysv_shmem_t sysv_shmem = sysv_shmem_t();

/**
 * A Handle is an object that can be converted into pointers even in different 
 * process memory spaces, provided that the pointed memory reside in shared
 * memory. To deal with the fact that a handle can be interpreted 
 * in different blocks of shared memory, we add an object's id to the handle to 
 * identify the block to which it belongs.
 */
class Handle {
	
	typedef boost::interprocess::managed_external_buffer::handle_t bh_t;
	bh_t value_; /*!< enclosed boost handle */
	int object_id_; /*!< object's id */
	
	public:

	Handle() : object_id_(-1) {}

	/**
	 * Constructor.
	 * 
	 * \param[in] h : boost handle.
	 * \param[in] id : shared memory segment id.
	 */
	Handle(bh_t h, int id=0)
	: value_(h), object_id_(id) {}

	/**
	 * Destructor.
	 */
	~Handle() {}

	/**
	 * Affectation from a boost handle.
	 */
	Handle& operator=(const bh_t& hdl)
	{
		value_ = hdl;
		object_id_ = 0;
		return *this;
	}
	
	/**
	 * Get the Id of the shared memory segment.
	 */
	int GetID() const {
		return object_id_;
	}
	
	/**
	 * Set the Id of the handle.
	 *
	 * \param[in] id : id of the shared memory segment.
	 */
	void SetID(int id) {
		object_id_ = id;
	}
	
	/**
	 * Returns the boost handle.
	 */
	bh_t GetValue() const {
		return value_;
	}
};

}

#endif
