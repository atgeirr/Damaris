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
 * \file LayoutManager.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_LAYOUT_MANAGER_H
#define __DAMARIS_LAYOUT_MANAGER_H

#include "xml/Model.hpp"
#include "data/Layout.hpp"
#include "core/Configurable.hpp"

namespace Damaris {

	/**
	 * LayoutManager keeps track of all Layout instance, and records them
	 * by name and by id.
	 */
	class LayoutManager : public Manager<Layout> {

	public:
		/**
		 * Initializes all the Layout instances from a given model (from XML).
		 */
		static void Init(const Model::Data& mdl);

	};
	
}

#endif
