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
#include "common/ParameterSet.hpp"
#include "common/VariableSet.hpp"
#include "common/Configurable.hpp"

namespace Damaris {

	/**
	 * MetadataManager holds pointers to all Variables.
	 * These variables can be retrieved by their name or by a unique ID.
	 */
	class MetadataManager : public Configurable<MetadataManager,Model::Data> {

	private:
		std::map<std::string,Layout> layouts; /*!< Map associating names with layouts. */

		VariableSet* variables;   /*!< Variables indexed by name and by id. */
		ParameterSet* parameters; /*!< Set of parameters defined in the configuration. */
		Environment* environment; /*!< A pointer to the Environment object hold by the Process. */

		Calc<std::string::const_iterator,ParameterSet>* layoutInterp; /*!< Calc instance to interpret a layout. */

		/**
		 * This function is called by the constructor to help initializing
		 * everything.
		 */
		void init();

		/**
		 * Go through a Model::Group instance and read variables recursively. 
		 * \param[in] g : a pointer to a Model::Group to parse.
		 * \param[in] groupName : the parent group name.
		 */
		void readVariablesInSubGroup(const Model::Group *g,
                        const std::string& groupName);

		/**
		 * Initialize the ParameterSet.
		 */
		void initParameters();

		/**
		 * Initialize the VariableSet.
		 */
		void initVariables();

		/**
		 * Initialize the set of Layout.
		 */
		void initLayouts();
	public:
		/**
		 * MetadataManager Constructor.
		 * \param[in] mdl : the base model to initialize everything.
		 * \param[in] env : the Environment object initialized by the Process.
		 */
		MetadataManager(Model::Data* mdl, Environment* env);

		/**
		 * Returns a reference to the inner VariableSet.
		 */
		VariableSet& getVariableSet() { return *variables;}

		/**
		 * Add a new variable entry within the metadata manager.
		 * An error is printed if a variable is already defined with the same name,
		 * or if the layout does not exist.
		 */
		bool addVariable(const std::string & varname, const std::string & layoutName);

		/**
		 * Add a new variable entry within the metadata manager.
		 */
		bool addVariable(const std::string & varname, const std::string & layoutName,
				 const std::string & description, const std::string & unit);
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
		 * \param[in] lname : name of the Layout as defined in the configuration file.
		 * \param[in] l : reference to an initialized Layout.
		 */
		bool addLayout(const std::string& lname, Layout &l);

		/**
		 * Retrieve a layout by name.
		 */
		Layout* getLayout(const std::string& lname);

		/**
		 * Print the list of defined variables in an outputstream.
		 * This function is for debugging.
		 */
		void listVariables(std::ostream &out);

		/**
		 * \brief Destructor.
		 */
		~MetadataManager();
	};
	
}

#endif
