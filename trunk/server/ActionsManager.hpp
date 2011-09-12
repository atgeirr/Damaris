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
 * \file ActionsManager.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#ifndef __DAMARIS_BEHAVIOR_H
#define __DAMARIS_BEHAVIOR_H

#include <map>
#include <string>

//#include "common/Configuration.hpp"
#include "common/MetadataManager.hpp"
#include "server/Action.hpp"

namespace Damaris {

/**
 * The ActionsManager keeps the list of defined actions
 * It is in charge of loading functions from shared libraries
 * and call functions.
 */
class ActionsManager {
	private:
		std::map<std::string,Action*> actions; /*!< Map associating event names to actions. */

	public:
		/**
		 * \brief Constructor.
		 */
		ActionsManager();
		
		/** 
		 * \brief Loads a function from a dynamic library.
		 * \param[in] eventName : Name of the event taken as a key for this action.
		 * \param[in] fileName : Name of the dynamic library (.so, .dylib...)
		 * \param[in] functionName : Name of the function to load in this dynamic library.
		 */
		void loadActionFromPlugin(std::string* eventName, std::string* fileName, std::string* functionName);

		/**
		 * \brief Call a function. In reaction to a fired event.
		 * \param[in] sig : name of the event.
		 * \param[in] iteration : iteration at which the event is sent.
		 * \param[in] sourceID : source that fired the event.
		 * \param[in,out] mm : Pointer to the MetadataManager.
		 */
		void reactToUserSignal(std::string* sig, int32_t iteration, int32_t sourceID, MetadataManager* mm);

};

}

#endif
