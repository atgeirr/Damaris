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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#ifndef __DAMARIS_LAYOUT_H
#define __DAMARIS_LAYOUT_H

#include <string>
#include <vector>
#include "core/Manager.hpp"
#include "data/Types.hpp"

namespace Damaris {

/**
 * Layouts are objects that describe the representation of data. 
 * Layouts are not used to hold data. See Chunk for that purpose.
 */	
class Layout {
	friend class Manager<Layout>;
		
	protected:
		int id; /*!< id of the Layout, provided when initialized by the LayoutManager. */
		std::string name; /*! name of the Layout, a simple copy of its name from the model. */

		/**
		 * \brief Constructor.
		 * Initializes a Layout from the data type, the dimensions and the vector of extents. 
		 * 
		 * \param[in] name : name of the layout (as defined in the configuration file).
		 */
		Layout(const std::string& name);
		
	public:
		/**
		 * \brief Returns the name of the Layout.
		 */
		virtual const std::string& GetName() const
		{ return name; }

		/**
		 * Returns the id of the Layout.
		 */
		virtual int GetID() const
		{ return id; }

		/**
		 * \return The type of the data. 
		 */
		virtual Model::Type GetType() const = 0;
		
		/**
		 * \return the number of dimensions. 
		 */
		virtual unsigned int GetDimensions() const = 0;

		/**
		 * \return the extent along a given dimension. 
		 */
		virtual size_t GetExtentAlongDimension(unsigned int dim) const = 0;

		/**
		 * \return true if the Layout has unlimited dimension 
		 *         (defined as "?" in the configuration file).
		 */
		virtual bool IsUnlimited() const = 0;

		/**
		 * Destructor.
		 */
		virtual ~Layout();
}; // class Layout

} // namespace Damaris

#endif
