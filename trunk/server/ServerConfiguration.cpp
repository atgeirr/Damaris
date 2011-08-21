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

#include <list>
#include <map>
#include <iostream>
#include <string>
#include <stdio.h>

#include "common/Debug.hpp"
#include "common/Language.hpp"
#include "common/Types.hpp"
#include "common/ConfigHandler.hpp"
#include "common/Configuration.hpp"
#include "server/ServerConfiguration.hpp"

namespace Damaris {
	
	ServerConfiguration* ServerConfiguration::m_instance = NULL;

	ServerConfiguration::ServerConfiguration(std::string *cfgFile) : Configuration(cfgFile)
	{
		actionsManager   = new ActionsManager();
		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		try {
			configHandler->readConfigFile(cfgFile);
		} catch(...) {
			exit(-1);
		}
		/* the ConfigHandler must be deleted afterward */
		delete configHandler;
		if(!(this->checkConfiguration())) exit(-1);
	}

	ServerConfiguration::~ServerConfiguration()
	{
		delete actionsManager;
	}
		
	ServerConfiguration* ServerConfiguration::getInstance()
	{
		return m_instance;
	}

	void ServerConfiguration::initialize(std::string* configFile)
	{
		if(m_instance) {
			WARN("Configuration already initialized.");
			return;
		}
		m_instance = new ServerConfiguration(configFile);
	}

	void ServerConfiguration::finalize()
	{
		if(m_instance) {
			delete m_instance;
			m_instance = NULL;
		} else {
			WARN("Configuration not initialized, cannot be finalized.");
		}
	}

	ActionsManager* ServerConfiguration::getActionsManager()
	{
		return actionsManager;
	}

	void ServerConfiguration::setEvent(const char* name, const char* action, const char* plugin)
	{
		std::string actionName(name);
		std::string actionFunc(action);
		std::string actionFile(plugin);
		INFO("In setEvent, registering " << name);
		actionsManager->loadActionFromPlugin(&actionName, &actionFile, &actionFunc);
	}
}

