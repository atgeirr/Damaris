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
 * \file ChunkHeader.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_CHUNK_HEADER_H
#define __DAMARIS_CHUNK_HEADER_H

#include <stdlib.h>
#include "xml/Model.hpp"
#include "data/Chunk.hpp"
#include "data/ChunkDescriptor.hpp"

namespace Damaris {

	/**
	 * The ChunkHeader class is used to store some information
	 * related to the Chunk inside a shared memory segment when passing
	 * a Chunk from a process to another.
	 */
	class ChunkHeader : public ChunkDescriptor {

		private:
			int source;     /*!< ID of the process that generated the chunk.*/
			int iteration;  /*!< iteration at which the chunk has been generated. */
			int block;	/*!< block ID. */
			Model::Type::value type; /*! Type of the Chunk. */

		public:
			ChunkHeader(const ChunkDescriptor* ch, const Model::Type& t, 
					int it, int src, int b = 0) 
				: ChunkDescriptor(*ch), 
				source(src), 
				iteration(it), 
				block(b), 
				type(t) {};

			/**
			 * \brief Gets the ID of the process that has written the chunk.
			 */
			int GetSource() const { return source; }

			/**
			 * \brief Set the ID of the process that has written the chunk.
			 */
			void SetSource(int src) { source = src;}

			/**
			 * \brief Gets the iteration at which the chunk has been written.
			 */
			int GetIteration() const { return iteration; }

			/**
			 * \brief Set the iteration number.
			 */
			void SetIteration(int i) { iteration = i; }

			/**
			 * \brief Get the ID of the block.
			 */
			int GetBlock() const { return block; }

			/**
			 * \brief Set the ID of the block.
			 */
			void SetBlock(int b) { block = b; }

			/**
			 * \brief Get the type of the chunk
			 */
			Model::Type GetType() const { return type; }

			/**
			 * \brief Set the type.
			 */
			void SetType(const Model::Type& t) { type = t; }
	};

} // namespace Damaris

#endif
