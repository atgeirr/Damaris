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

#ifndef __DAMARIS_SERVER_CONFIG_H
#define __DAMARIS_SERVER_CONFIG_H

#include <map>
#include <list>
#include <string>

#include "common/Configuration.hpp"
#include "server/ActionsManager.hpp"

namespace Damaris {

	/** 
	 * The class Configuration holds all informations extrated from
	 * both the command lines (for the server) and the external configuration
	 * file. It is independant of the format of this configuration file
	 * (which is loaded through the ConfigHandler object).
	 */
	class ServerConfiguration : public Configuration {
		
	private:
		static ServerConfiguration* m_instance;
		
		ActionsManager* actionsManager; /*!< Container of actions. */
	
		/**
		 * \brief Constructor.
		 * \param[in] configName : name of the configuration file to load.
		 */
		ServerConfiguration(std::string* configName);

		/**
		 * \brief Destructor.
		 */
		~ServerConfiguration();

	public:
		/**
		 * \brief Retrieve an instance of Configuration (singleton design pattern).
		 * \return NULL if Configuration::initialize has never been called before, a valid pointer otherwise.
		 */
		static ServerConfiguration* getInstance();

		/**
		 * \brief Initializes Configuration with a given configuration file.
		 * \param[in] configName : name of the configuration file to load.
		 */
		static void initialize(std::string* configName);
		
		/**
		 * \brief Finalize (free resources) Configuration.
		 * If Configuration::getInstance() is called after finalize, NULL is returned.
		 */
		static void finalize();
		
		/**
		 * \brief Get the ActionManager initialized within the configuration.
		 * Only available for Server.
		 * \return The ActionManager.
		 */
		ActionsManager* getActionsManager();
		
		void setEvent(const char* name, const char* action, const char* plugin);
};

}

#endif
