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
 * \file Layout.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * This file defines the Layout object.
 */
#ifndef __DAMARIS_LAYOUT_H
#define __DAMARIS_LAYOUT_H

#include <string>
#include <valarray>
#include <vector>
#include <stdint.h>

#include "common/Types.hpp"

namespace Damaris {

/**
 * Layouts are objects that describe the representation of
 * data in memory. For example "an 3D array of integers which extents are
 * 4,2 and 16". Actually we choosed to represent layouts as in Fortran, using
 * a starting index and an ending index rather than a size. This way allows 
 * plugins to potentially know that the variable is a chunk of a bigger
 * global array.
 */	
class Layout {
		
	private:
		Types::basic_type_e type; 	/*!< Type of the data. */
		int32_t dimensions; 		/*!< Number of dimensions. */
		int64_t* startIndex; 		/*!< Start index along each dimension. */
		int64_t* endIndex; 		/*!< End index along each dimencion. */
		
	public:
		/**
		 * \brief Constructor.
		 * Initializes a Layout from the data type, the dimensions and the vector of extents. 
		 * 
		 * \param[in] t : basic type.
		 * \param[in] d : number of dimensions.
		 * \param[in] extents : list of extents, even indices hold starting indices, 
		 *                      non-even hold ending indices.
		 */
		Layout(Types::basic_type_e t, int32_t d, std::vector<int64_t> extents);
		
		/** 
		 * \brief Constructor.
		 * Initializes a Layout without knowing its extents.
		 *
		 * \param[in] t : basic type.
		 * \param[in] d = number of dimensions.
		 */
		Layout(Types::basic_type_e t, int32_t d);
		
		/**
		 * \brief Destructor.
		 */
		~Layout();
		
		/**
		 * \return Size of the buffer that would hold the data.
		 */
		size_t  getRequiredMemoryLength() const;
	
		/**
		 * \return The type of the data. 
		 */
		Types::basic_type_e getType() const;
		
		/**
		 * \return the number of dimensions. 
		 */
		int32_t getDimensions() const;
		
		/**
		 * \return the starting index along a particular dimension.
		 */
		int64_t getStartIndex(int dim) const;
		
		/**
		 * \return the ending index along a particular dimension. 
		 */
		int64_t getEndIndex(int dim) const;
		
		/**
		 * \return the extent (start-end+1) along a given dimension. 
		 */
		int64_t getExtentAlongDimension(int dim) const;
		
}; // class Layout
	
} // namespace Damaris

#endif
