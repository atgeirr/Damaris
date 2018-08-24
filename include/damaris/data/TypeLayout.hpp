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

#ifndef __DAMARIS_TYPE_LAYOUT_H
#define __DAMARIS_TYPE_LAYOUT_H

#include <string>

#include "damaris/util/Configurable.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Unused.hpp"
#include "damaris/util/ForwardDcl.hpp"

#include "damaris/data/Type.hpp"
#include "damaris/data/Layout.hpp"

namespace damaris {

/**
 * TypeLayout represents a Layout derived from a basic type, it
 * avoids for the user the need to define the layout for simple variables.
 */	
class TypeLayout : public Layout, 
		   public Configurable<model::Type> {
			   
	friend class Deleter<Layout>;
	friend class Deleter<TypeLayout>;
		
	private:

	model::Type type_;
	
	/**
	 * Constructor.
	 * 
	 * \param[in] mdl : model from which to define the TypeLayout.
	 */
	TypeLayout(const model::Type& mdl)
	: Layout((std::string)mdl), Configurable<model::Type>(type_), type_(mdl)
	{}
		
	/**
	 * Destructor.
	 */
	virtual ~TypeLayout() {}
		
	public:
	
	/**
	 * Returns the type of the data. 
	 */
	virtual model::Type GetType() const {
		return GetModel();
	}
		
	/**
	 * Returns the number of dimensions. 
	 */
	virtual unsigned int GetDimensions() const {
		return 1;
	}

	/**
 	* Returns the extent along a given dimension.
 	* For this class, the returned value is always 1.
 	*/
	virtual size_t GetExtentAlong(unsigned int UNUSED(dim) , bool includeGhost = true) const {
		return 1;
	}

	/**
	 * \see Layout::GetGlobalExtentAlong
	 */
	virtual size_t GetGlobalExtentAlong(unsigned int dim) const {
		return 1;
	}

	/**
	 * \see Layout::GetGhostAlong
	 */
	virtual std::pair<size_t,size_t> GetGhostAlong(unsigned int dim) const {
		return std::make_pair<size_t,size_t>(0,0);
	}
	
	/**
	 * \see Layout::GetRequiredMemory()
	 */
	virtual size_t GetRequiredMemory(bool withGhost=true) const {
		return TypeSize(GetModel());
	}

	/**
	* \see Layout::GetNumberOfItems()
	*/
	virtual size_t GetNumberOfItems(bool withGhost=true) const {
		return 1;
	}
		
	/**
	 * Creates a new TypeLayout from a model.
	 * 
	 * \param[in] mdl : model from which to create the TypeLayout.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Type& mdl, 
			const std::string& UNUSED(name)) 
	{
		return std::shared_ptr<SUPER>(new TypeLayout(mdl), 
					 Deleter<TypeLayout>());
	}
	
};

}

#endif
