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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_CHUNK_H
#define __DAMARIS_CHUNK_H

#include <stdlib.h>
#include <vector>
#include "common/Types.hpp"
#include "common/Layout.hpp"
#include "common/Serializable.hpp"

namespace Damaris {

/**
 * The Chunk class is an abstract class defining
 * the extents and types of a chunk of variable.
 * It inherits from Serializable so its representation
 * can be written into a buffer (used to pass this
 * representation from a process to another using
 * shared memory).
 */
class Chunk : public Serializable {
		
	private:
		Types::basic_type_e type; /*!< Type of the data. */
		unsigned int dimensions;  /*!< Number of dimensions. */

		std::vector<int> startIndices; /*!< Start indices. */
		std::vector<int> endIndices;   /*!< End indices. */

	protected:
		int source; 	/*!< ID of the process that generated the chunk.*/
		int iteration; 	/*!< iteration at which the chunk has been generated. */

		/**
		 * \brief Constructor.
		 * This constructor is protected so only child classes can call it.
		 */
		Chunk();
	
		/**
		 * \brief Constructor.
		 * \param[in] t : type of data.
		 * \param[in] d : number of dimensions.
		 * \param[in] si : start indices.
		 * \param[in] ei : end indices.
		 * The size of ei and si must match the number of dimensions d.
		 * Otherwise a warning is print and the extents and dimensions are set to 0.
		 * This constructor is protected so only child classes can call it.
		 */
		Chunk(Types::basic_type_e t, unsigned int d, std::vector<int> &si, std::vector<int> &ei);

	public:
		/**
		 * \brief Destructor.
		 */
		~Chunk();
		
		/**
		 * \brief Gets the ID of the process that has written the chunk.
		 */
		int getSource() const;
	
		/**
		 * \brief Gets the iteration at which the chunk has been written.
		 */
		int getIteration() const;

		/**
		 * \brief Gets the number of dimensions.
		 */
		unsigned int getDimensions();

		/**
		 * \brief Gets the type of data.
		 */
		Types::basic_type_e getType();

		/**
		 * \brief Gets a start index.
		 */
		int getStartIndex(int i);
		
		/**
		 * \brief Gets an end index.
		 */
		int getEndIndex(int i);

		/**
		 * \brief Computes the required number of bytes to allocate for the data.
		 */
		size_t getDataMemoryLength();

		/**
		 * \brief Check if the chunk is within an enclosing Layout.
		 * Note: returns false if NULL is passed.
		 */
		bool within(Layout* enclosing);

		/**
		 * \brief Check if the chunk is within an enclosing other Chunk.
		 * Note: returns false if NULL is passed.
		 */
		bool within(Chunk* enclosing);

		/**
		 * \brief Overloaded function for the Serializable interface.
		 * Writes the chunk representation into a buffer.
		 */
		void toBuffer(void*);

		/**
		 * \brief Overloaded function for the Serializable interface.
		 * Reads the chunk from its serialized representation.
		 */
		void fromBuffer(const void*);

		/**
		 * \brief Overloaded function for the Serializable interface.
		 * Returns the size of the buffer required to serialize the representation.
		 * \warning The size returned does not take into account the size required
		 *          for the data, only the size of the metadata (type, dimensions,
		 *          extents, source and iteration).
		 */
		size_t size();

		/**
		 * \brief Returns a pointer over the actual data (to be overloaded in child classes).
		 */
		virtual void* data() = 0;

		/**
		 * \brief Removes (if possible) the data from its underlying storage.
		 */
		virtual bool remove() = 0;

}; // class Chunk
	
} // namespace Damaris

#endif
