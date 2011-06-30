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
 * \file Action.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#include "common/Action.hpp"

namespace Damaris {
/*	
	Action::Action()
	{
		function = NULL;
	}
*/	
	// main constructor
	Action::Action(void(*fptr)(const std::string*, int32_t, int32_t, MetadataManager*))
	{
		function = fptr;
	}
	
	Action::~Action()
	{}
	
	void Action::operator()(const std::string* event, 
				  int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(function != NULL)
			(*function)(event,iteration,sourceID,mm);
	}
	
	void Action::call(const std::string* event,
				int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(function != NULL)
			(*function)(event,iteration,sourceID,mm);
	}
	
}
