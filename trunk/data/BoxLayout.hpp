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
 * \file BoxLayout.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#ifndef __DAMARIS_BOX_LAYOUT_H
#define __DAMARIS_BOX_LAYOUT_H

#include <string>
#include <vector>
#include "core/Observer.hpp"
#include "core/Calc.hpp"
#include "core/ParameterManager.hpp"
#include "core/Configurable.hpp"
#include "data/Types.hpp"
#include "data/Layout.hpp"

namespace Damaris {

/**
 * BoxLayouts are objects that describe the representation of data. 
 * For example "a 3D array of integers which extents are 4,2 and 16".
 * BoxLayouts are represented C-like, each index along each dimension
 * starting from 0 and going to n-1, where n is the extent of the
 * array along that dimension.
 *
 * BoxLayouts are not used to hold data. See Chunk for that purpose.
 */	
class BoxLayout : public Layout, public Configurable<Model::Layout>, private Observer {
		
	private:
		std::vector<int> extents;	/*!< Extents along each dimension. */

		static Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> > *calc;

		/**
		 * \brief Constructor.
		 * Initializes a Layout from the data type, the dimensions and the vector of extents. 
		 *
		 * \param[in] mdl : base model from configuration file. 
		 * \param[in] name : name of the layout (as defined in the configuration file).
		 * \param[in] e : list of extents.
		 * \param[in] blocks : number of blocks.
		 */
		BoxLayout(const Model::Layout& mdl, const std::string& name, 
			  const std::vector<int>& e);
		
		/**
		 * Interpret the dimensions from the model's description.
		 * Can be called to change the dimensions if some parameters have been changed.
		 */
		void InterpretDimensions();

		/**
		 * Called by the constructor to connect to dependent parameters
		 * and be notified if they change.
		 */
		void ObserveDependentParameters();
	public:
		/**
		 * \return The type of the data. 
		 */
		virtual Model::Type GetType() const;
		
		/**
		 * \return the number of dimensions. 
		 */
		virtual unsigned int GetDimensions() const;

		/**
		 * \return the extent along a given dimension. 
		 */
		virtual size_t GetExtentAlongDimension(unsigned int dim) const;

		/**
		 * \return true if the Layout has unlimited dimension 
		 *         (defined as "?" in the configuration file).
		 */
		virtual bool IsUnlimited() const;

		/**
		 * Tries to create a new Layout from a model and a name.
		 * Returns NULL in case of failure.
		 */
		static Layout* New(const Model::Layout& mdl, const std::string &name);

		/**
		 * Overwrite the Notify function of Observer. Causes the layout to
		 * be rebuilt when a parameter has changed.
		 */
		virtual void Notify();

		/**
		 * Destructor.
		 */
		virtual ~BoxLayout();
}; // class Layout

} // namespace Damaris

#endif
