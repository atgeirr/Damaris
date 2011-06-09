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
#ifdef __DAMARIS_SERVER
		actionsManager   = new ActionsManager();
#endif
		/* initializing the parameters list */
		parameters = new std::map<std::string,Parameter>();
		/* initializing the layouts list */
		layouts = new std::map<std::string,Layout*>();
		/* initializing the list of variables layouts */
		variableLayouts = new std::map<std::string,std::string>();
		/* here we create the ConfigHandler to load the xml file */
		Damaris::ConfigHandler *configHandler = new Damaris::ConfigHandler(this);
		configHandler->readConfigFile(configFile);
		/* the ConfigHandler must be deleted afterward */
		delete configHandler;
		if(!(this->checkConfiguration())) exit(-1);
	}

	Configuration::~Configuration()
	{
		delete layouts;
		delete parameters;
		delete configFile;
		delete msgQueueName;
		delete segmentName;
#ifdef __DAMARIS_SERVER
		delete actionsManager;
#endif
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

#ifdef __DAMARIS_SERVER	
	ActionsManager* Configuration::getActionsManager()
	{
		return actionsManager;
	}
#endif

	void Configuration::setParameter(const char* name, const char* type, const char* value)
	{
		std::string paramName(name);
		Parameter paramValue;
		
		int readSuccessful = 1;

		if(strcmp(type,"short") ==0) {
			paramValue.type = SHORT;
			short* val = new short(0);
			int v;
			readSuccessful = sscanf(value,"%d",&v);
			*val = (short)v;
			paramValue.value.short_ptr = val;
		} else
		if(strcmp(type,"int") == 0) {
			paramValue.type = INT;
			int* val = new int(0);
			readSuccessful = sscanf(value,"%d",val);
			paramValue.value.int_ptr = val;
		} else
		if(strcmp(type,"long") == 0) {
			paramValue.type = LONG;
			long* val = new long(0);
			readSuccessful = sscanf(value,"%ld",val);
			paramValue.value.long_ptr = val;
		} else
		if(strcmp(type,"float") == 0) {
			paramValue.type = FLOAT;
			float* val = new float(0.0);
			readSuccessful = sscanf(value,"%f",val);
			paramValue.value.float_ptr = val;
		} else
		if(strcmp(type,"double")) {
			paramValue.type = DOUBLE;
			double* val = new double(0.0);
			readSuccessful = sscanf(value,"%lf",val);
			paramValue.value.double_ptr = val;
		} else
		if(strcmp(type,"char")) {
			paramValue.type = CHAR;
			char* val = new char();
			readSuccessful = sscanf(value,"%c",val);
			paramValue.value.char_ptr = val;
		} else
		if(strcmp(type,"string")) {
			paramValue.type = STR;
			std::string* val = new std::string(value);
			paramValue.value.str_ptr = val;
		} else {
			paramValue.type = INT; // just so the compiler doesn't shout
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
	
	void Configuration::setVariableInfo(const char* name, const char* layoutName)
	{
		std::map<std::string,Layout*>::iterator it;
		it = layouts->find(std::string(layoutName));
		if(it == layouts->end()) {
			WARN("When parsing variable \"" << name << "\", layout is not defined yet");
			return; 
		}

		variableLayouts->insert(std::pair<std::string,std::string>(std::string(name),std::string(layoutName)));
		INFO("Defining variable informations for \"" << name <<"\", layout is \""<< layoutName << "\"");
	}

	void Configuration::setLayout(const char* name, const char* type, const std::list<int>* dims, language_e l) 
	{
		std::string layoutName(name);
		std::string layoutType(type);
		std::list<int> d(*dims);
		std::vector<int64_t> extents(2*(dims->size()));

		if(l == LG_FORTRAN)
		{
			std::list<int>::reverse_iterator rit = d.rbegin();
			for(int i=0; rit != d.rend(); rit++, i++) 
			{
				extents[2*i] = 0;
				extents[2*i+1] = (int64_t)(*rit)-1;
			}
		} else {
			std::list<int>::const_iterator it = d.begin();
			for(int i=0; it != d.end(); it++, i++)
			{
				extents[2*i] = 0;
				extents[2*i+1] = (int64_t)(*it)-1;
			}
		}
		basic_type_e t = getTypeFromString(&layoutType);
		Layout* layout = new Layout(t,dims->size(),extents);
		std::pair<std::string,Layout*> ly(layoutName,layout);
		layouts->insert(ly);
		INFO("Layout \"" << name << "\" now defined");
	}

	Layout* Configuration::getLayout(const char* name)
	{
		std::map<std::string,Layout*>::iterator it;
		it = layouts->find(std::string(name));
                
		if(it == layouts->end()) {
			return NULL;
		}
		
		return it->second;
	}

	Layout* Configuration::getVariableLayout(const char* varName)
	{
		std::map<std::string,std::string>::iterator it;
		it = variableLayouts->find(std::string(varName));
		if(it == variableLayouts->end()) {
			return NULL;
		}
		
		return getLayout((it->second).c_str());
	}

	int Configuration::getParameterType(const char* name, basic_type_e* t)
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
			basic_type_e t = (it->second).type;
			switch(t) {
			case(SHORT) :
				memcpy(v,(it->second).value.short_ptr,sizeof(short)); break;
			case(INT) :
				memcpy(v,(it->second).value.int_ptr,sizeof(int)); break;
			case(LONG) :
				memcpy(v,(it->second).value.long_ptr,sizeof(long)); break;
			case(FLOAT) : 
				memcpy(v,(it->second).value.float_ptr,sizeof(float)); break;
			case(DOUBLE) :
				memcpy(v,(it->second).value.double_ptr,sizeof(double)); break;
			case(CHAR) :
				memcpy(v,(it->second).value.char_ptr,sizeof(char)); break;
			default :
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
#ifdef __DAMARIS_SERVER 
		std::string actionName(name);
		std::string actionFunc(action);
		std::string actionFile(plugin);
		INFO("In setEvent, registering " << name);
		actionsManager->loadActionFromPlugin(&actionName, &actionFile, &actionFunc);
#endif	
	}
}

