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

#ifndef __DAMARIS_LAYOUT_MANAGER_H
#define __DAMARIS_LAYOUT_MANAGER_H

#include "damaris/util/Manager.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/data/Layout.hpp"

namespace damaris {

/**
 * LayoutManager keeps track of all Layout instances, and records them by name 
 * and by id.
 */
class LayoutManager : public Manager<Layout> {

	public:
		
	/**
	 * Initializes all the Layout instances from a given model.
	 * This function will first call AddSimpleTypes().
	 *
	 * \param[in] mdl : model from which to fill the LayoutManager.
	 */
	static void Init(const model::Data& mdl);
	
	private:
		
	/**
	 * Add the simple types (int, float, double, etc.) in the LayoutManager.
	 */
	static void AddSimpleTypes();

	/**
	 * Recursively find layours in subgroups.
	 */
	static void readLayoutsInSubGroup(const model::Group &g,
			const std::string& groupName);

};
	
}

#endif
