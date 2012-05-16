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
 * \file MetadataManager.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_METADATA_H
#define __DAMARIS_METADATA_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "xml/Model.hpp"
#include "common/Calc.hpp"
#include "common/Layout.hpp"
#include "common/Variable.hpp"
#include "common/Environment.hpp"
#include "common/Configurable.hpp"

namespace Damaris {

	/**
	 * VariableManager holds pointers to all Variables.
	 * These variables can be retrieved by their name or by a unique ID.
	 */
	class VariableManager : public Manager<Variable,Model::Variable> {

	public:
		static void Init(const Model::Data& mdl);

	private:
		/**
		 * Go through a Model::Group instance and read variables recursively. 
		 * \param[in] g : a pointer to a Model::Group to parse.
		 * \param[in] groupName : the parent group name.
		 */
		static void readVariablesInSubGroup(const Model::Group &g,
                        const std::string& groupName);

	};
}

#endif
