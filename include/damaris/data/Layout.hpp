/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef __DAMARIS_LAYOUT_H
#define __DAMARIS_LAYOUT_H

#include <string>
#include <vector>

#include "damaris/util/Pointers.hpp"
//#include "damaris/util/TypeWrapper.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"

#include "damaris/model/Model.hpp"

namespace damaris {

/**
 * The Layout class is used to describe the shape of data in memory:
 * number of dimensions, extents along each dimensions, type, etc.
 * Layouts are kept in the LayoutManager.
 */
class Layout
{
	friend class Manager<Layout>;
	friend class Deleter<Layout>;
		
	protected:
	int id_; /*!< id of the Layout, initialized by the LayoutManager. */
	std::string name_; /*! name of the Layout, a simple copy of its 
				name from the model. */

	/**
	 * Initializes a Layout from its name.
	 * 
	 * \param[in] name : name of the layout.
	 */
	Layout(const std::string& name)
	: id_(-1), name_(name) {}
		
	/**
	 * Destructor.
	 */
	virtual ~Layout() {}
		
	public:
		
	/**
	 * Returns the name of the Layout.
	 */
	virtual const std::string& GetName() const { 
		return name_; 
	}

	/**
	 * Returns the id of the Layout.
	 */
	virtual int GetID() const { 
		return id_; 
	}

	/**
	 * Returns the type of the data. 
	 */
	virtual model::Type GetType() const = 0;
		
	/**
	 * Returns the number of dimensions. 
	 */
	virtual unsigned int GetDimensions() const = 0;

	/**
	 * Returns the extent along a given dimension.
	 *
	 * \param[in] dim : dimension along which to get the extent.
	 */
	virtual size_t GetExtentAlong(unsigned int dim) const = 0;

	/**
	 * Returns the global extent along a given dimension.
	 *
	 * \param[in] dim : dimension along which to get the extent.
	 */
	virtual size_t GetGlobalExtentAlong(unsigned int dim) const = 0;

	/**
	 * Returns the pair of ghost regions along a given dimension.
	 *
	 * \param[in] dim : dimension along which to get the ghost.
	 */
	virtual std::pair<size_t,size_t> GetGhostAlong(unsigned int dim) const = 0;
	
	/**
	 * Returns the size in memory of a dataset following this layout.
	 */
	virtual size_t GetRequiredMemory(bool includeGhost=true) const = 0;
		
}; // class Layout

} // namespace Damaris

#endif
