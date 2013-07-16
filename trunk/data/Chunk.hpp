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
 * \file Chunk.hpp
 * \date Oct. 2012
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_CHUNK_H
#define __DAMARIS_CHUNK_H

#include <stdlib.h>
#ifdef __ENABLE_VISIT
#include <VisItDataInterface_V2.h>
#endif

#include "xml/Model.hpp"
#include "memory/SharedMemory.hpp"
#include "data/Types.hpp"
#include "data/Layout.hpp"
#include "data/ChunkHeader.hpp"
#include "memory/Buffer.hpp"
#include "data/DataSpace.hpp"

namespace Damaris {

	/**
	 */
	class Chunk {

		private:
			bool isOwner; /*!< Indicates if this instance is the owner of the data
					(multiple instances can hold a pointer to the same data. */
			Buffer* buffer; /*!< Pointer to the shared memory 
							segment containing the data. */
			ChunkHeader* header; /*!< Pointer to header. */
			//void* addr; /*!< Pointer to the actual data. */
			DataSpace* space;
		public:

			/**
			 * Initialize a ShmChunk from a SharedMemorySegment and
			 * a pointer to an existing header in the process's memory.
			 */
			Chunk(Buffer* b, ChunkHeader* ch);

			/**
			 * Initialize a ShmChunk from a SharedMemorySegment and
			 * a pointer to an existing header in the process's memory,
			 * given as a handle.
			 */
			Chunk(Buffer* b, handle_t ch);

			/**
			 * \brief Destructor.
			 * If this instance is the owner of the data, the data will be deleted.
			 */
			virtual ~Chunk();

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
			 */
			void* Data();

			/**
			 * Get the DataSpace in the Chunk.
			 */
			DataSpace* GetDataSpace() const { return space; }

			/**
			 * Set the DataSpace associated to the Chunk.
			 * Will try to delete any previously attached DataSpace.
			 */
			void SetDataSpace(DataSpace* ds) {
				if(space != NULL) {
					delete space;
				}
				space = ds;
			}

			/**
			 * Gives the number of items contained in the Chunk. 
			 */
			int NbrOfItems() const;

			/**
			 * \brief Check if the chunk is within an enclosing other Chunk. 
			 */
			bool Within(const Chunk& enclosing) const;

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
			 * Returns a relative pointer (handle) to the memory region where the 
			 * chunk is located. Warning: this handle points to the header, not the data.
			 */
			handle_t GetHandle();

#ifdef __ENABLE_VISIT
			/**
			 * Fills a VisIt data handle (already allocated) to expose the data
			 * to VisIt. Return true if it managed to expose the data.
			 */
			bool FillVisItDataHandle(visit_handle hdl);
#endif

			virtual void DownScaleToLimits();

	}; // class Chunk

} // namespace Damaris

#endif

