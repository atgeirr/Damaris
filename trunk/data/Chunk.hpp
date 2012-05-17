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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_CHUNK_H
#define __DAMARIS_CHUNK_H

#include <stdlib.h>
#include "xml/Model.hpp"
#include "data/Types.hpp"
#include "data/Layout.hpp"

namespace Damaris {
/**
 * The Chunk class is an abstract class defining
 * the extents and types of a chunk of variable.
 * It inherits from Serializable so its representation
 * can be written into a buffer (used to pass this
 * representation from a process to another using
 * shared memory).
 */
class Chunk {
	public:		
		/**
         * \brief Gets the ID of the process that has written the chunk.
         */
        virtual int getSource() const = 0;

		/**
		 * \brief Set the ID of the process that has written the chunk.
		 */
		virtual void setSource(int src) = 0;

		/**
		 * \brief Gets the iteration at which the chunk has been written.
		 */
		virtual int getIteration() const = 0;

		/**
		 * \brief Set the iteration number.
		 */
		virtual void setIteration(int i) = 0;

		/**
		 * \brief Returns a pointer over the actual data (to be overloaded in child classes).
		 */
		virtual void* data() = 0;

		/**
		 * \brief Removes (if possible) the data from its underlying storage.
		 */
		virtual bool remove() = 0;

		/**
         * \brief Gets the number of dimensions.
         */
        virtual unsigned int getDimensions() const = 0;

        /**
         * \brief Gets the type of data.
         */
        virtual Model::Type getType() const = 0;

        /**
         * \brief Gets a start index.
         */
        virtual int getStartIndex(int i) const = 0;

        /**
         * \brief Gets an end index.
         */
        virtual int getEndIndex(int i) const = 0;

        /**
         * \brief Check if the chunk is within an enclosing Layout.
         * Note: returns false if NULL is passed.
         */
        bool within(const Layout& enclosing) const;

        /**
         * \brief Check if the chunk is within an enclosing other Chunk.
         * Note: returns false if NULL is passed.
         */
		bool within(const Chunk& enclosing) const;

}; // class Chunk
	
} // namespace Damaris

#endif
