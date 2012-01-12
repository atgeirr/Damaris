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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * MetadataManager holds pointers to all Variables published.
 * These variables can be retrieved by their identifier (name,source,iteration).
 */
#ifndef __DAMARIS_METADATA_H
#define __DAMARIS_METADATA_H

#include <iostream>
#include <map>
#include <vector>

#include "common/Layout.hpp"
#include "common/Variable.hpp"
#include "common/VariableSet.hpp"
#include "common/Singleton.hpp"

namespace Damaris {

	class MetadataManager : public Singleton<MetadataManager> {
		friend class Singleton<MetadataManager>;
	private:
		std::map<std::string,Layout> layouts; /*!< Map associating names with layouts. */
		VariableSet variables; /*!< Variables indexed by name and by id. */

		/**
		 * Constructor.
		 */
		MetadataManager();

	public:
		VariableSet& getVariableSet() { return variables;}

		/**
		 * Add a new variable entry within the metadata manager.
		 * An error is printed if a variable is already defined with the same name,
		 * or if the layout does not exist.
		 */
		bool addVariable(const std::string & varname, const std::string & layoutName);

		/**
		 * Retrieve a variable from its name.
		 */
		Variable* getVariable(const std::string &name);

		/**
		 * Retrieve a variable by ID.
		 */
		Variable* getVariable(int id);

		/**
		 * Adds a layout.
		 */
		bool addLayout(const std::string& lname, Layout &l);

		/**
		 * Retrieve a layout by name.
		 */
		Layout* getLayout(const std::string& lname);

		void listVariables(std::ostream &out);

		/**
		 * \brief Destructor.
		 * \warning The destructor does not free the
		 * variables or their associated chunks.
		 */
		~MetadataManager();
	};
	
}

#endif
