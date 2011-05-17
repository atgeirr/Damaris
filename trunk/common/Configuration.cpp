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
	
	Configuration::Configuration(std::string *cfgFile)
	{
		configFile = new std::string(*cfgFile);
		/* here we create the ConfigHandler to load the xml file */
		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		configHandler->readConfigFile(configFile);
		/* the ConfigHandler must be deleted afterward */
		delete configHandler;
	}
}

