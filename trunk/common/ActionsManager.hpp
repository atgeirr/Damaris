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

#ifndef __DAMARIS_BEHAVIOR_H
#define __DAMARIS_BEHAVIOR_H

#include <map>
#include <string>

#include "common/MetadataManager.hpp"
#include "common/Action.hpp"

namespace Damaris {

/**
 * The ActionsManager keeps the list of defined actions
 * It is in charge of loading functions from shared libraries
 * and call functions.
 */
class ActionsManager {
	private:
		// map associating event names to actions
		std::map<std::string,Action*> actions;
		// pointer to the metadata manager
		//MetadataManager *metadataManager;

	public:
		// constructor, takes a pointer to the metadata manager
		ActionsManager();//MetadataManager* mm);
		// loads a function from a dynamic library
		void loadActionFromPlugin(std::string* eventName, std::string* fileName, std::string* functionName);
		// call a function
		void reactToSignal(std::string* sig, int32_t iteration, int32_t sourceID, MetadataManager* mm);
};

}

#endif
