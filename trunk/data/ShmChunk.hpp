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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_SHMCHUNK_H
#define __DAMARIS_SHMCHUNK_H

#include <stdlib.h>

#include "data/Chunk.hpp"
#include "data/ChunkHeader.hpp"
#include "core/SharedMemorySegment.hpp"

namespace Damaris {

/**
 * The ShmChunk class inherites from Chunk, it represents
 * a chunk located in shared memory.
 */
class ShmChunk : public Chunk {

	private:
		SharedMemorySegment* segment; /*!< Pointer to the shared memory 
										segment containing the data. */
		ChunkHeader* header; /*!< Pointer to header. */
		void* buffer; /*!< Pointer to the actual data. */
	public:
		/**
		 * Initialize a ShmChunk from a SharedMemorySegment and
		 * a pointer to an existing header in the process's memory.
		 */
		ShmChunk(SharedMemorySegment* s, ChunkHeader* ch);

		/**
		 * Initialize a ShmChunk from a SharedMemorySegment and
         * a pointer to an existing header in the process's memory,
		 * given as a handle.
         */
		ShmChunk(SharedMemorySegment* s, handle_t ch);
		/**
		 * \brief Destructor.
		 * Does not free the associated header.
		 */
		~ShmChunk();
		/**
         * \brief Gets the ID of the process that has written the chunk.
         */
        virtual int getSource() const { return header->getSource(); }

        /**
         * \brief Set the ID of the process that has written the chunk.
         */
        virtual void setSource(int src) { header->setSource(src); }

        /**
         * \brief Gets the iteration at which the chunk has been written.
         */
        virtual int getIteration() const { return header->getIteration(); }

        /**
         * \brief Set the iteration number.
         */
        virtual void setIteration(int i) { header->setIteration(i); }

		/**
         * \brief Gets the number of dimensions.
         */
        virtual unsigned int getDimensions() const { return header->getDimensions(); }

        /**
         * \brief Gets the type of data.
         */
        virtual Model::Type getType() const { return header->getType(); }

        /**
         * \brief Gets a start index.
         */
        virtual int getStartIndex(int i) const { return header->getStartIndex(i); }

        /**
         * \brief Gets an end index.
         */
        virtual int getEndIndex(int i) const { return header->getEndIndex(i); }

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

		/**
		 * Returns a relative pointer (handle) to the memory region where the 
		 * chunk is located. Warning: this handle points to the header, not the data.
		 */
		handle_t getHandle();

}; // class ShmChunk
	
} // namespace Damaris

#endif

