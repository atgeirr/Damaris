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
 * \file ShmChunk.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * This file defines the ShmChunk class,
 * a Chunk placed in shared memory.
 */
#ifndef __DAMARIS_SHMCHUNK_H
#define __DAMARIS_SHMCHUNK_H

#include <stdlib.h>
#include "common/Chunk.hpp"
#include "common/SharedMemorySegment.hpp"

namespace Damaris {

/**

 */
class ShmChunk : public Chunk {

	private:
		SharedMemorySegment* segment; /*!< Pointer to the shared memory segment containing the data. */
		void* header; /*!< Pointer to the header in the shared segment (where the layout is described. */
		void* buffer; /*!< Pointer to the actual data in shared memory. */

	public:
		/**
		 * \brief Constructor.
		 * \param[in] s  : Pointer to the associated shared memory segment.
		 * \param[in] t  : Type of data to be handled.
		 * \param[in] d  : number of dimensions.
		 * \param[in] si : starting indices along each dimension.
		 * \param[in] ei : ending indices along each dimension.
		 * This function also allocate the required memory in the shared memory segment.
		 * This function may throw an exception if it doesn't manage to open the memory region
		 * or if a wrong shared memory segment argument (for instance "NULL") is passed.
		 */
		ShmChunk(SharedMemorySegment* s, Types::basic_type_e t, unsigned int d, std::vector<int> &si, std::vector<int> &ei);
		
		/**
		 * \brief Constructor from an existing chunk placed in shared memory.
		 * \param[in] s : Pointer to the associated shared memory segment.
		 * \param[in] header : Pointer (handle) to the header in shared memory.
		 * This function builds the ShmChunk from an already existing representation
		 * in shared memory. The required memory is not allocated.
		 */
		ShmChunk(SharedMemorySegment* s, handle_t header);

		/**
		 * \brief Destructor.
		 */
		~ShmChunk();
		
		/**
		 * \brief Retrieves a pointer to the data.
		 * \return A pointer to where the data is stored, or NULL if the data
		 *         has been removed by this object using remove().
		 * \warning If several instances of ShmChunk exist (possibly in different processes)
		 *          and the program calls "remove" on one of them, the other will be
		 *          inconsistant. If an instance of ShmChunk is deleted without removing
		 *          the shared memory region before, the region will be lost. Thus make sure 
		 *          there is always eventually exactly one ShmChunk instance attached to a
		 *          region.
		 */
		void* data();
		
		/**
		 * \brief Removes the data shared memory.
		 * \return true if successfuly removed, false otherwise.
		 * \warning See ShmChunk::data().
		 */
		bool remove();

		handle_t getHandle();

		void setIteration(int i);
		
		void setSource(int s);
}; // class ShmChunk
	
} // namespace Damaris

#endif

