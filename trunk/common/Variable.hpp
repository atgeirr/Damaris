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
	
	class Variable {
	private:
		std::string* name;   // name of the variable
		int64_t iteration;   // iteration of publication
		int32_t sourceID; // source that published this variable
		Layout* layout;     // layout of the data
		void* data;     // pointer on the data
	
	public:
		Variable(std::string* vname, int32_t it, int32_t src, Layout* l, void* d);
		std::string* getName() const;
		int32_t getIteration() const;
		int32_t getSource() const;
		Layout* getLayout() const;
		void* getDataAddress() const;
		bool compare(const Variable &v) const;
		void print() const;
		void setDataToNull(); 
	};
	
}

#endif
