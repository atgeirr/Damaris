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
 * \file SharedMemory.hpp
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * Contains some definitions useful for shared memory support.
 */
#ifndef __DAMARIS_SHARED_MEMORY_H
#define __DAMARIS_SHARED_MEMORY_H
#include <stdint.h>

namespace Damaris {

	struct posix_shmem_t { };
	struct sysv_shmem_t { };

	static const struct posix_shmem_t posix_shmem = posix_shmem_t();
	static const struct sysv_shmem_t sysv_shmem = sysv_shmem_t();
	
	typedef int64_t handle_t;
}

#endif
