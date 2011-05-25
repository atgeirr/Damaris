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
#include <list>
#include <iostream>
#include <stdio.h>

#include "common/Debug.hpp"
#include "common/Language.hpp"
#include "common/ConfigHandler.hpp"
#include "common/Configuration.hpp"

namespace Damaris {
	
	Configuration::Configuration(std::string *cfgFile)
	{
		configFile = new std::string(*cfgFile);
		simulationName 	= NULL;
		coresPerNode 	= -1;
		segmentName 	= NULL;
		segmentSize	= 0;
		msgQueueName	= NULL;
		msgQueueSize	= 0;
		defaultLanguage = LG_UNKNOWN;

		/* initializing the parameters list */
		parameters = new std::map<std::string,Parameter>();
		/* here we create the ConfigHandler to load the xml file */
		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		configHandler->readConfigFile(configFile);
		/* the ConfigHandler must be deleted afterward */
		delete configHandler;
		if(!(this->checkConfiguration())) exit(-1);
	}

	Configuration::~Configuration()
	{
		delete parameters;
		delete configFile;
		delete msgQueueName;
		delete segmentName;
	}

	bool Configuration::checkConfiguration()
	{
		bool res = true;
		if(segmentName == NULL) {
			ERROR("Buffer name not set.");
			res = false;
		}
		if(segmentSize == 0) {
			ERROR("Buffer size not set.");
			res = false;
		}
		if(msgQueueName == NULL) {
			ERROR("Message queue name not set.");
			res = false;
		}
		if(msgQueueSize == 0) {
			ERROR("Message queue size not set.");
			res = false;
		}
		return res;
	}

	void Configuration::setParameter(const char* name, const char* type, const char* value)
	{
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
		INFO("The parameter \"" << name << "\" of type \"" << type << "\" has been set to the value " << value);
	}
	
	void Configuration::setVariable(const char* name, const char* layoutName)
	{
		INFO("Defining variable " << name);
	}

	void Configuration::setLayout(const char* name, const char* type, const std::list<int>* dims, language_e l) 
	{
		INFO("Defining layout " << name);
	}

	int Configuration::getParameterType(const char* name, param_type_e* t)
	{
		std::map<std::string,Parameter>::iterator i;
		i = parameters->find(std::string(name));
		if(i == parameters->end())
			return 0;
		else
			*t = (i->second).type;
		return 1;
	}
	
	int Configuration::getParameterValue(const char* name, void* v)
	{
		std::map<std::string,Parameter>::iterator it;
		it = parameters->find(std::string(name));
		if(it == parameters->end())
			return 0;
		else
		{
			param_type_e t = (it->second).type;
			switch(t) {
			case(PARAM_INT) :
				memcpy(v,(it->second).value.int_ptr,sizeof(int)); break;
			case(PARAM_LONG) :
				memcpy(v,(it->second).value.long_ptr,sizeof(long)); break;
			case(PARAM_FLOAT) : 
				memcpy(v,(it->second).value.float_ptr,sizeof(float)); break;
			case(PARAM_DOUBLE) :
				memcpy(v,(it->second).value.double_ptr,sizeof(double)); break;
			case(PARAM_CHAR) :
				memcpy(v,(it->second).value.char_ptr,sizeof(char)); break;
			case(PARAM_STR) :
				/* */ break;
			/* TODO : all other types including string shouldn't be used */
			}
		}
		return 1;
	}

	int Configuration::getParameterString(const char* name, std::string* s)
	{
		/* TODO */
		return 0;
	}

	void Configuration::setEvent(const char* name, const char* action, const char* plugin)
	{
		/* TODO */
	}
}

