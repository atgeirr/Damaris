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
#include <map>
#include <iostream>
#include <stdio.h>

#include "common/Debug.hpp"
#include "common/ConfigHandler.hpp"
#include "common/Configuration.hpp"

namespace Damaris {
	
	Configuration::Configuration(std::string *cfgFile)
	{
		configFile = new std::string(*cfgFile);
		/* initializing the parameters list */
		parameters = new std::map<std::string,Parameter>();
		/* here we create the ConfigHandler to load the xml file */
		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		configHandler->readConfigFile(configFile);
		/* the ConfigHandler must be deleted afterward */
		delete configHandler;
	}

	Configuration::~Configuration()
	{
		delete parameters;
		delete configFile;
	}

	void Configuration::setParameter(char* name, char* type, char* value)
	{
		INFO("The parameter \"" << name << "\" of type \"" << type << "\" has been set to the value " << value);
		std::string paramName(name);
		Parameter paramValue;
		
		int readSuccessful = 1;

		if(strcmp(type,"int")) {
			paramValue.type = PARAM_INT;
			int* val = new int(0);
			readSuccessful = sscanf(value,"%d",val);
			paramValue.value.int_ptr = val;
		} else
		if(strcmp(type,"long")) {
			paramValue.type = PARAM_LONG;
			long* val = new long(0);
			readSuccessful = sscanf(value,"%ld",val);
			paramValue.value.long_ptr = val;
		} else
		if(strcmp(type,"float")) {
			paramValue.type = PARAM_FLOAT;
			float* val = new float(0.0);
			readSuccessful = sscanf(value,"%f",val);
			paramValue.value.float_ptr = val;
		} else
		if(strcmp(type,"double")) {
			paramValue.type = PARAM_DOUBLE;
			double* val = new double(0.0);
			readSuccessful = sscanf(value,"%lf",val);
			paramValue.value.double_ptr = val;
		} else
		if(strcmp(type,"char")) {
			paramValue.type = PARAM_CHAR;
			char* val = new char();
			readSuccessful = sscanf(value,"%c",val);
			paramValue.value.char_ptr = val;
		} else
		if(strcmp(type,"string")) {
			paramValue.type = PARAM_STR;
			std::string* val = new std::string(value);
			paramValue.value.str_ptr = val;
		} else {
			paramValue.type = PARAM_INT; // just so the compiler doesn't shout
			ERROR("Unknown type \"" << type << "\" for parameter \"" << name << "\"");
			return;
		}

		if(readSuccessful != 1) {
			ERROR("While parsing parameter's value, unable to interprete value for parameter \"" << name << "\"");
			return;
		}
		parameters->insert( std::pair<std::string,Parameter>(paramName,paramValue) );
	}
	
	void Configuration::setVariable(char* name, char* layoutName)
	{
		INFO("defining variable " << name);
	}
}

