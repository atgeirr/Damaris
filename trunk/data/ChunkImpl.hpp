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
 * \file ChunkImpl.hpp
 * \date Oct. 2012
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_CHUNKIMPL_H
#define __DAMARIS_CHUNKIMPL_H

#include <stdlib.h>

#include "data/Chunk.hpp"
#include "data/ChunkHeader.hpp"
#include "memory/Buffer.hpp"

namespace Damaris {

	/**
	 * The ChunkImpl class inherites from Chunk, it represents
	 * a chunk located in memory.
	 */
	class ChunkImpl : public Chunk {

		private:
			bool isOwner; /*!< Indicates if this instance is the owner of the data
					(multiple instances can hold a pointer to the same data. */
			Buffer* buffer; /*!< Pointer to the shared memory 
							segment containing the data. */
			ChunkHeader* header; /*!< Pointer to header. */
			void* addr; /*!< Pointer to the actual data. */
		public:

			/**
			 * Initialize a ShmChunk from a SharedMemorySegment and
			 * a pointer to an existing header in the process's memory.
			 */
			ChunkImpl(Buffer* b, ChunkHeader* ch);

			/**
			 * Initialize a ShmChunk from a SharedMemorySegment and
			 * a pointer to an existing header in the process's memory,
			 * given as a handle.
			 */
			ChunkImpl(Buffer* b, handle_t ch);

			/**
			 * \brief Destructor.
			 * If this instance is the owner of the data, the data will be deleted.
			 */
			virtual ~ChunkImpl();

			/**
			 * \brief Gets the ID of the process that has written the chunk.
			 */
			virtual int GetSource() const { return header->GetSource(); }

			/**
			 * \brief Set the ID of the process that has written the chunk.
			 */
			virtual void SetSource(int src) { header->SetSource(src); }

			/**
			 * \brief Gets the iteration at which the chunk has been written.
			 */
			virtual int GetIteration() const { return header->GetIteration(); }

			/**
			 * \brief Set the iteration number.
			 */
			virtual void SetIteration(int i) { header->SetIteration(i); }

			/**
			 * \brief Get the ID of the block.
			 */
			virtual int GetBlock() const { return header->GetBlock(); }

			/**
			 * \brief Set the ID of the block.
			 */
			virtual void SetBlock(int b) { header->SetBlock(b); }

			/**
			 * \brief Gets the number of dimensions.
			 */
			virtual unsigned int GetDimensions() const { return header->GetDimensions(); }

			/**
			 * \brief Gets the type of data.
			 */
			virtual Model::Type GetType() const { return header->GetType(); }

			/**
			 * \brief Gets a start index.
			 */
			virtual int GetStartIndex(int i) const { return header->GetStartIndex(i); }

			/**
			 * \brief Gets an end index.
			 */
			virtual int GetEndIndex(int i) const { return header->GetEndIndex(i); }

			/**
			 * \brief Retrieves a pointer to the data.
			 * \return A pointer to where the data is stored, or NULL if the data
			 *         has been removed by this object using remove().
			 * \warning If several instances of ShmChunk exist (possibly in different processes)
			 *          and the program deletes an instance of ShmChunk which is the owner of the data,
			 * 			the other will be inconsistant. 
			 *			If no instance of ShmChunk is the owner of their corresponding data,
			 *          the shared memory region will be lost when these chunks are deleted. 
			 *			Thus make sure there is always eventually exactly 
			 *			one ShmChunk instance attached to a region which is owner of the data.
			 */
			void* Data();

			/**
			 * Indicates whether or not this instance of chunk is responsible for deleting
			 * the data it points to.
			 */
			virtual void SetDataOwnership(bool b)
			{ isOwner = b; }

			/**
			 * Returns the data ownership of this instance of chunk.
			 */
			virtual bool GetDataOwnership()
			{ return isOwner; }

			/**
			 * Copy data from a pointer.
			 */
			virtual size_t MemCopy(const void* src);

			/**
			 * Returns a relative pointer (handle) to the memory region where the 
			 * chunk is located. Warning: this handle points to the header, not the data.
			 */
			handle_t GetHandle();

	}; // class ShmChunk

} // namespace Damaris

#endif

