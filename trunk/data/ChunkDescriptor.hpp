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
 * \file ChunkDescriptor.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_CHUNK_DESC_H
#define __DAMARIS_CHUNK_DESC_H

#include <stdlib.h>
#include "xml/Model.hpp"
#include "data/Layout.hpp"
#include "data/Types.hpp"

namespace Damaris {

	/**
	 * The ChunkDescriptor object is used to describe the shape
	 * of a chunk : number of dimensions, lower and upper bounds
	 * for each dimension, type of the data.
	 */
	class ChunkDescriptor {
		private:
			static const int MAX_DIM = 16; /*!< The maximum number of dimensions is fixed to 32. */

		protected:
			unsigned int dimensions;  /*!< Number of dimensions. */
			int lbounds[MAX_DIM];  /*!< Lower bounds of the chunk. */
			int ubounds[MAX_DIM];  /*!< Upper bounds of the chunk. */

			/**
			 * Constructor taking the number of dimensions, lower and upper bounds.
			 */
			ChunkDescriptor(unsigned int d, const int* lb, const int* ub);

			/**
			 * Constructor from a Layout.
			 */
			ChunkDescriptor(const Layout& l);		

			/**
			 * Copy constructor, also copies the bound arrays.
			 */
			ChunkDescriptor(const ChunkDescriptor& ch);

		public:
			/**
			 * External Constructor.
			 */
			static ChunkDescriptor* New(unsigned int d, const int* lb, const int* ub);

			/**
			 * External Constructor.
			 */
			static ChunkDescriptor* New(const Layout& l);

			/**
			 * External Destructor.
			 */
			static void Delete(ChunkDescriptor* cd);

			/**
			 * \brief Computes the required number of bytes to allocate for the data.
			 */
			size_t getDataMemoryLength(const Model::Type& t) const;

			/**
			 * \brief Gets the number of dimensions.
			 */
			unsigned int getDimensions() const { return dimensions; }

			/**
			 * \brief Gets a start index.
			 */
			int getStartIndex(int i) const;

			/**
			 * \brief Gets an end index.
			 */
			int getEndIndex(int i) const;

			/**
			 * \brief Check if the chunk is within an enclosing Layout.
			 * Note: returns false if NULL is passed.
			 */
			bool within(const Layout& enclosing) const;

	}; // class ChunkDescriptor

} // namespace Damaris

#endif
