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
	/* The constructor takes the name of the variable, the iteration t, the source id src
	   the layout l and a pointer to the data (d) */
	Variable::Variable(std::string n, int32_t t, int32_t src, Layout* l, void* d)
	{
		//name = new std::string(*n);
		name = n;
		iteration = t;
		source = src;
		layout = l;
		data = d;
	}
/*	
	bool Variable::compare(const Variable &v) const
	{
		bool c = true;
		c = c && (v.getName()->compare(*name) == 0);
		c = c && (v.getSource() == sourceID);
		c = c && (v.getIteration() == iteration);
		return c;
	}
*/
/*	
	void Variable::print() const 
	{
		std::cout << *name << ", iteration = " << iteration << " source = " << sourceID << std::endl; 
	}
*/
	bool Variable::operator==(const Variable &another)
	{
		bool c = true;
		c = c && another.name.compare(name) == 0;
		c = c && another.source == source;
		c = c && another.iteration == iteration;
		return c;
	}
}
