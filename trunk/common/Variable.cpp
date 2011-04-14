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

#include <iostream>

#include "common/Variable.hpp"

namespace Damaris {
	
	Variable::Variable(std::string* n, int32_t t, int32_t src, Layout* l, void* d)
	{
		name = new std::string(*n);
		iteration = t;
		sourceID = src;
		layout = l;
		data = d;
	}
	
	std::string* Damaris::Variable::getName() const
	{
		return name;
	}
	
	int32_t Damaris::Variable::getIteration() const
	{
		return iteration;
	}
	
	int32_t Damaris::Variable::getSource() const
	{
		return sourceID;
	}
	
	Layout* Variable::getLayout() const
	{
		return layout;
	}

	void* Variable::getDataAddress() const
	{
		return data;
	}
	
	bool Variable::compare(const Variable &v) const
	{
		bool c = true;
		c = c && (v.getName()->compare(*name) == 0);
		c = c && (v.getSource() == sourceID);
		c = c && (v.getIteration() == iteration);
		return c;
	}
	
	void Variable::print() const 
	{
		std::cout << *name << ", iteration = " << iteration << " source = " << sourceID << std::endl; 
	}

	void Variable::setDataToNull()
	{
		data = NULL;
	}
}
