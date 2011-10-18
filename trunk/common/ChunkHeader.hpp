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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * This file defines the Chunk object.
 */
#ifndef __DAMARIS_CHUNK_HEADER_H
#define __DAMARIS_CHUNK_HEADER_H

#include <stdlib.h>
#include "common/Types.hpp"

namespace Damaris {

/**

 */	
struct ChunkHeader {
		
		Types::basic_type_e type; 	/*!< Type of the data. */
		unsigned int dimensions; 	/*!< Number of dimensions. */
		int* startIndex; 		/*!< Start index along each dimension. */
		int* endIndex; 			/*!< End index along each dimencion. */
		
		/**
		 * \brief Constructor.
		 * Initializes a Layout from the data type, the dimensions and the vector of extents. 
		 * 
		 * \param[in] t : basic type.
		 * \param[in] d : number of dimensions.
		 * \param[in] extents : list of extents, even indices hold starting indices, 
		 *                      non-even hold ending indices.
		 */
		ChunkHeader(Types::basic_type_e t, unsigned int d);

		ChunkHeader(Types::basic_type_e t, unsigned int d, int* sIndex, int* eIndex);

		ChunkHeader(const ChunkHeader &h);
		//ChunkHeader(ChunkHeader& h);					
		/**
		 * \brief Destructor.
		 */
		~ChunkHeader();
		
		size_t size();

		void toBuffer(void* dest) const;

		static ChunkHeader fromBuffer(void* src);
		
		/**
		 * \return Size of the buffer that would hold the data.
		 */
		size_t  getRequiredMemoryLength() const;
	
		/**
		 * \return the extent (start-end+1) along a given dimension. 
		 */
		int getExtentAlongDimension(unsigned int dim) const;
		
}; // class Chunk
	
} // namespace Damaris

#endif

