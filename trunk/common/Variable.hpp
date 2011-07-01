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
 * \file Variable.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * Variable.hpp contains the definition of the Variable record.
 */
#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <stdint.h>
#include <string>
#include "common/Layout.hpp"

namespace Damaris {

/**
 * The Variable object is used for describing a variable within
 * a metadata structure. It holds a pointer to the data and additional
 * informations. A Variable record is identified by a name, an iteration
 * and a source.
 */
struct Variable {
		
		std::string name;	/*!< Name of the variable. */
		int32_t iteration;	/*!< Iteration of publication. */
		int32_t source;		/*!< Source that published this variable. */
		Layout* layout;		/*!< Layout of the data. */
		void* data;		/*!< Pointer to the data. */
	
		/**
		 * \brief Constructor. 
		 * Creates a Variable record given a name, an iteration, a source, a layout
		 * and a pointer to the data.
		 * 
		 * \param[in] vname : Name of the variable.
		 * \param[in] it : Iteration of the variable.
		 * \param[in] src : Source of the variable.
		 * \param[in] l : Layout that physically describe the data.
		 * \param[in] d : Pointer to the data.
		 */
		Variable(std::string vname, int32_t it, int32_t src, Layout* l, void* d);

		// All the following accessors are useless since we use a structure with public members


		/* returns the layout of the variable */
		/* Layout* getLayout() const { return layout; } */
		/* returns the pointer over the data */
		/* void* getDataAddress() const { return data; } */
		/* compares the records (except data and layout) with another variable */
		//bool compare(const Variable &v) const;
		/* print informations related to the variable */
		// void print() const;
		/* reset the pointer to the data */
		//void setDataToNull() { data = NULL; }

		/**
		 * \brief Comparison operator between variables.
		 * Variables are equals if they have a same name, iteration and source.
		 */
		bool operator==(const Variable &another);
};

}

#endif
