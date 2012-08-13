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
#include "core/Observer.hpp"
#include "core/Calc.hpp"
#include "core/ParameterManager.hpp"
#include "core/Configurable.hpp"
#include "data/Types.hpp"

namespace Damaris {

/**
 * Layouts are objects that describe the representation of data. 
 * For example "a 3D array of integers which extents are 4,2 and 16".
 * Layouts are represented C-like, each index along each dimension
 * starting from 0 and going to n-1, where n is the extent of the
 * array along that dimension.
 *
 * Layouts are not used to hold data. See Chunk for that purpose.
 */	
class Layout : public Configurable<Model::Layout>, private Observer {
	friend class Manager<Layout>;
		
	private:
		int id; /*!< id of the Layout, provided when initialized by the LayoutManager. */
		std::string name; /*! name of the Layout, a simple copy of its name from the model. */
		std::vector<int> extents;	/*!< Extents along each dimension. */	

		static Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> > *calc;

		/**
		 * \brief Constructor.
		 * Initializes a Layout from the data type, the dimensions and the vector of extents. 
		 * 
		 * \param[in] name : name of the layout (as defined in the configuration file).
		 * \param[in] t : basic type.
		 * \param[in] d : number of dimensions.
		 * \param[in] extents : list of extents.
		 */
		Layout(const Model::Layout& mdl, const std::string& name, const std::vector<int>& e);
		

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
		 * \brief Returns the name of the Layout.
		 */
		const std::string& getName() const;

		/**
		 * Returns the id of the Layout.
		 */
		int getID() const;	

		/**
		 * \return The type of the data. 
		 */
		Model::Type getType() const;
		
		/**
		 * \return the number of dimensions. 
		 */
		unsigned int getDimensions() const;

		/**
		 * \return the extent along a given dimension. 
		 */
		size_t getExtentAlongDimension(unsigned int dim) const;

		/**
		 * \return true if the Layout has unlimited dimension 
		 *         (defined as "?" in the configuration file).
		 */
		bool isUnlimited() const;

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
		virtual ~Layout();
}; // class Layout

} // namespace Damaris

#endif
