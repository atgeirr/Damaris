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

#ifndef __DAMARIS_BOX_LAYOUT_H
#define __DAMARIS_BOX_LAYOUT_H

#include <string>
#include <vector>

#include "damaris/util/Observer.hpp"
#include "damaris/util/Calc.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/util/Deleter.hpp"

#include "damaris/data/ParameterManager.hpp"
#include "damaris/data/Type.hpp"
#include "damaris/data/Layout.hpp"

namespace damaris {

/**
 * BoxLayouts are objects that describe the representation of data. 
 * For example "a 3D array of integers which extents are 4,2 and 16".
 * BoxLayouts are represented C-like, each index along each dimension
 * starting from 0 and going to n-1, where n is the extent of the
 * array along that dimension.
 *
 * BoxLayouts are not used to hold data. See Block and DataSpace 
 * for that purpose. These objects inherit from Observer and observe the
 * parameters they depend on.
 */	
class BoxLayout : public Layout, 
		  public Configurable<model::Layout>, 
		  public Observer,
		  public ENABLE_SHARED_FROM_THIS(BoxLayout) {
	
	friend class Deleter<BoxLayout>;
			  
private:
	std::vector<int> extents_;	/*!< Extents along each dimension. */
	std::vector<int> global_extents_; /*!< Global extents along each dimension. */
	std::vector<std::pair<size_t,size_t> > ghosts_; /*!< Ghost zones along each dimension. */

	typedef Calc<std::string::const_iterator,
			ParameterManager::ParameterMap<int> > CalcType;

	static std::shared_ptr<CalcType> _calc_; /*!< This object calculates the
		layout from a string representation that includes parameters. */

	/**
	* Initializes a Layout from the data type, the dimensions and 
	* the vector of extents. 
	*
	* \param[in] mdl : base model from configuration file. 
	*/
	BoxLayout(const model::Layout& mdl)
	: Layout(mdl.name()), Configurable<model::Layout>(mdl) 
	{
		DBG("Creating layout " << GetName());
	}

	/**
	 * Destructor.
	 */
	virtual ~BoxLayout() {
		DBG("Destroying layout " << GetName());
	}
		
	/**
	 * Interpret the dimensions from the model's description.
	 * Can be called to change the dimensions if some parameters 
	 * have been changed.
	 */
	void InterpretDimensions();

	/**
	 * Called by the constructor to connect to dependent parameters
	 * and be notified if they change.
	 */
	bool ObserveDependentParameters();
	
	public:
		
	/**
	 * Get the type of the data. 
	 */
	virtual model::Type GetType() const {
		return GetModel().type();
	}
		
	/**
	 *  Get the number of dimensions. 
	 */
	virtual unsigned int GetDimensions() const {
		return extents_.size();
	}

	/**
	 * Get the extent along a given dimension.
	 *
	 * \param[in] dim : dimension along which to get the extent.
	 */
	virtual size_t GetExtentAlong(unsigned int dim , bool includeGhost = true) const {
		if(dim < GetDimensions())
            if (includeGhost)
			    return extents_[dim];
            else
                return extents_[dim] - ghosts_[dim].first - ghosts_[dim].second;
		else
			return 1;
	}


	/**
	 * \see Layout::GetGlobalExtentAlong
	 */
	virtual size_t GetGlobalExtentAlong(unsigned int dim) const {
		if(dim < GetDimensions())
			return global_extents_[dim];
		else
			return 1;
	}

	/**
	 * \see Layout::GetGhostAlong
	 */
	virtual std::pair<size_t,size_t> GetGhostAlong(unsigned int dim) const {
		if(dim < GetDimensions())
			return ghosts_[dim];
		else
			return std::make_pair<size_t,size_t>(0,0);
	}
	
	/**
	 * Get the size (in bytes) of the memory required to store a block
	 * following this layout.
	 */
	virtual size_t GetRequiredMemory(bool withGhost=true) const {
		size_t s = TypeSize(GetModel().type());
		for(unsigned int i=0; i<extents_.size(); i++) {
			if(withGhost) {
				s *= GetExtentAlong(i);
			} else {
				size_t e = GetExtentAlong(i);
				std::pair<size_t,size_t> g = GetGhostAlong(i);
				e -= g.first;
				e -= g.second;
				s *= e;
			}
		}
		return s;
	}

	/**
	 * Overwrite the Notify function of Observer. Causes the layout to
	 * be rebuilt when a parameter has changed.
	 */
	virtual void Notify() {
		InterpretDimensions();
	}
	
	/**
	 * Tries to create a new Layout from a model and a name.
	 * Returns NULL in case of failure.
	 * 
	 * \param[in] mdl : base model from which to create the layout.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Layout& mdl, 
				     const std::string& name) {

		if(mdl.name().find("/") != std::string::npos) {                 
			CFGERROR("Layout " << mdl.name() << " cannot have a '/' character.");
			return std::shared_ptr<SUPER>();                
		}

		std::shared_ptr<BoxLayout> p 
			= std::shared_ptr<BoxLayout>(new BoxLayout(mdl), 
					 Deleter<BoxLayout>());
		p->name_ = name;
		if(not p->ObserveDependentParameters()) {
			return std::shared_ptr<SUPER>();
		}
		p->InterpretDimensions();
		return std::shared_ptr<SUPER>(p);
	}
	
}; // class Layout

} // namespace damaris

#endif
