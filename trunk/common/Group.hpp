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
 * \file Group.hpp
 * \date August 2011
 * \author Matthieu Dorier
 * \version 0.2
 *
 * Contains the definition of a Group.
 */
#ifndef __DAMARIS_GROUP_H
#define __DAMARIS_GROUP_H

#include <string>
#include <map>

namespace Damaris {

/**
 */
class Group {
	private:
		std::string* name;	/*!< Name of the group. */
		Group* parent;		/*!< Parent group. */
		std::map<std::string,Group*> children; /*!< Children groups. */
	
	public:	
		Group(const char* gname);

		Group(Group* p, const char* gname);

		~Group();

		std::string getName() {return *name;}

		void setParent(Group* p) { parent = p;}
		
		Group* getParent() {return parent;}

		void addChild(Group* g);

		void addChild(const char* cname, Group* g);

		Group* getChild(const char* cname);
};

}

#endif
