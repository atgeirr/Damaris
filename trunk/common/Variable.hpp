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

#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <stdint.h>
#include <string>
#include "common/Layout.hpp"

namespace Damaris {

/**
 * The Variable object is used for describing a variable within
 * a metadata structure. It has a pointer to the data and additional
 * informations.
 */
class Variable {
	private:
		std::string* name;	// name of the variable
		int64_t iteration;	// iteration of publication
		int32_t sourceID;	// source that published this variable
		Layout* layout;		// layout of the data
		void* data;		// pointer on the data
	
	public:
		/* Constructor */
		Variable(std::string* vname, int32_t it, int32_t src, Layout* l, void* d);
		/* returns the name of the variable */
		std::string* getName() const;
		/* returns the iteration at which the variable has been created */
		int32_t getIteration() const { return iteration; }
		/* returns the source (client id) which published the variable */
		int32_t getSource() const { return sourceID; }
		/* returns the layout of the variable */
		Layout* getLayout() const { return layout; }
		/* returns the pointer over the data */
		void* getDataAddress() const { return data; }
		/* compares the records (except data and layout) with another variable */
		bool compare(const Variable &v) const;
		/* print informations related to the variable */
		// void print() const;
		/* reset the pointer to the data */
		void setDataToNull() { data = NULL; }
};
	
}

#endif
