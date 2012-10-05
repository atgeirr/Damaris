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
 * \file TypeLayout.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#ifndef __DAMARIS_TYPE_LAYOUT_H
#define __DAMARIS_TYPE_LAYOUT_H

#include <string>
#include <vector>
#include "core/Configurable.hpp"
#include "data/Types.hpp"
#include "data/Layout.hpp"

namespace Damaris {

/**
 * TypeLayouts represent a Layout derived from a basic type, it
 * avoids for the user the need to define the layout for simple variables.
 */	
class TypeLayout : public Layout, public Configurable<Model::Type> {
		
	private:
		/**
		 * \brief Constructor.
		 */
		TypeLayout(const Model::Type& mdl);
		
	public:
		/**
		 * \return The type of the data. 
		 */
		virtual Model::Type getType() const;
		
		/**
		 * \return the number of dimensions. 
		 */
		virtual unsigned int getDimensions() const;

		/**
		 * \return the extent along a given dimension. 
		 */
		virtual size_t getExtentAlongDimension(unsigned int dim) const;

		/**
                 * \return the number of blocks expected to be written.
                 */
                virtual unsigned int getBlocks() const;

		/**
		 * \return true if the Layout has unlimited dimension 
		 *         (defined as "?" in the configuration file).
		 */
		virtual bool isUnlimited() const;

		/**
		 * Tries to create a new Layout from a model and a name.
		 * Returns NULL in case of failure.
		 */
		static Layout* New(const Model::Type& mdl, const std::string &name);

		/**
		 * Destructor.
		 */
		virtual ~TypeLayout();
}; // class Layout

} // namespace Damaris

#endif
