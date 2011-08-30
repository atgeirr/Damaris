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
 * \file Group.cpp
 * \date August 2011
 * \author Matthieu Dorier
 * \version 0.2
 *
 * Contains the definition of a Group.
 */

#include "common/Group.hpp"

namespace Damaris {

	Group::Group(const char* gname)
	{
		name = new std::string(gname);
		parent = NULL;
	}

	Group::Group(Group* p, const char* gname)
	{
		name = new std::string(gname);
		parent = p;
	}

	Group::~Group()
	{
		delete name;
		std::map<std::string,Group*>::iterator it;
		for(it = children.begin(); it != children.end(); it++)
			delete (*it).second;
	}
	
	void Group::addChild(Group* g)
	{
		children.insert(std::pair<std::string,Group*>(std::string(g->getName()),g));
	}

	void Group::addChild(const char* cname, Group* g)
	{
		children.insert(std::pair<std::string,Group*>(std::string(cname),g));
	}

	Group* Group::getChild(const char* cname)
	{
		return children[std::string(cname)];
	}
}

