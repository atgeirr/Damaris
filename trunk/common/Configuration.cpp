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

#include <string>
#include <iostream>
#include "common/Debug.hpp"
#include "common/ConfigHandler.hpp"
#include "common/Configuration.hpp"

namespace Damaris {
	// this is for a potentiel future Singleton implementation
	// of the object, when multithread support will ve enabled	
	Configuration* Configuration::config = NULL;	

	Configuration* Configuration::_config = NULL;	

	Configuration* Configuration::getInstance()
	{
		if(_config == NULL)
			return new Configuration();
		else
			return _config;
	}

	Configuration* Configuration::getInstance(std::string *cfgFile)
	{
		if(_config == NULL)
			return new Configuration(cfgFile);
		else
			return _config;
	}

	Configuration::Configuration()
	{
		Damaris::Configuration::_config = this;
	}
	
	Configuration::Configuration(std::string *cfgFile)
	{
		configFile = new std::string(*cfgFile);

		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		configHandler->readConfigFile(configFile);
		
		delete configHandler;
		Damaris::Configuration::config = this;
	}

	int Configuration::getNodeID() const
	{
		return id;
	}
	
}

