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

#include "common/Calc.hpp"
#include "common/Debug.hpp"
#include "common/Language.hpp"
#include "common/Types.hpp"
#include "common/Configuration.hpp"

namespace Damaris {

	Configuration* Configuration::m_instance = NULL;
		
	Configuration::Configuration(std::auto_ptr<Model::simulation_mdl> mdl, std::string *cfgFile)
	{
		configFile = new std::string(*cfgFile);
		baseModel = mdl;
		metadataManager = NULL;
		actionsManager = NULL;
//		layouts = new std::map<std::string,Layout*>();
		/* initializing the list of variables layouts */
//		variableLayouts = new std::map<std::string,std::string>();
		/* initialize the layout interpretor */
		fillParameterSet();
		layoutInterp = new Calc<std::string::const_iterator,ParameterSet>(parameters);
	}

	Configuration::~Configuration()
	{
		//delete layouts;
		delete configFile;
		delete layoutInterp;
	}

	void Configuration::fillParameterSet()
	{
		Model::data_mdl::parameter_const_iterator p(baseModel->data().parameter().begin());
		for(; p < baseModel->data().parameter().end(); p++) {
			if(p->type() == "int") {
				std::string name(p->name());
				int value = boost::lexical_cast<int>(p->value());
				parameters.set<int>(name,value);
			} else {
				WARN("Parameter other than integers are not currently allowed");
			}
		}
	}
/*		
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
*/
/*
	void Configuration::setParameter(const char* name, const char* type, const char* value)
	{
		std::string paramName(name);
		Parameter paramValue;
		
		int readSuccessful = 1;

		if(strcmp(type,"short") ==0) {
			paramValue.type = Types::SHORT;
			short* val = new short(0);
			int v;
			readSuccessful = sscanf(value,"%d",&v);
			*val = (short)v;
			paramValue.value.short_ptr = val;
			intparams.insert(std::pair<std::string,int>(std::string(name),(int)v));
		} else
		if(strcmp(type,"int") == 0) {
			paramValue.type = Types::INT;
			int* val = new int(0);
			readSuccessful = sscanf(value,"%d",val);
			paramValue.value.int_ptr = val;
			intparams.insert(std::pair<std::string,int>(std::string(name),*val));
		} else
		if(strcmp(type,"long") == 0) {
			paramValue.type = Types::LONG;
			long* val = new long(0);
			readSuccessful = sscanf(value,"%ld",val);
			paramValue.value.long_ptr = val;
		} else
		if(strcmp(type,"float") == 0) {
			paramValue.type = Types::FLOAT;
			float* val = new float(0.0);
			readSuccessful = sscanf(value,"%f",val);
			paramValue.value.float_ptr = val;
		} else
		if(strcmp(type,"double")) {
			paramValue.type = Types::DOUBLE;
			double* val = new double(0.0);
			readSuccessful = sscanf(value,"%lf",val);
			paramValue.value.double_ptr = val;
		} else
		if(strcmp(type,"char")) {
			paramValue.type = Types::CHAR;
			char* val = new char();
			readSuccessful = sscanf(value,"%c",val);
			paramValue.value.char_ptr = val;
		} else
		if(strcmp(type,"string")) {
			paramValue.type = Types::STR;
			std::string* val = new std::string(value);
			paramValue.value.str_ptr = val;
		} else {
			paramValue.type = Types::INT; // just so the compiler doesn't shout
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
*/
/*	
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
*/
/*
	void Configuration::setLayout(const char* name, const char* type, const char* description, language_e l) 
	{
		std::string layoutName(name);
		std::string layoutType(type);
		std::vector<int> dims;

		std::string str(description);
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		
		bool r = phrase_parse(iter, end, *layoutInterp, boost::spirit::ascii::space, dims);
		if((!r) || (iter != end)) {
			ERROR("While parsing dimension descriptor for layout \"" << name << "\"");
			return;
		}
		
		std::vector<int64_t> extents(2*dims.size());

		if(l == LG_FORTRAN)
		{
			std::vector<int>::reverse_iterator rit = dims.rbegin();
			for(int i=0; rit != dims.rend(); rit++, i++) 
			{
				extents[2*i] = 0;
				extents[2*i+1] = (int64_t)(*rit)-1;
			}
		} else {
			std::vector<int>::const_iterator it = dims.begin();
			for(int i=0; it != dims.end(); it++, i++)
			{
				extents[2*i] = 0;
				extents[2*i+1] = (int64_t)(*it)-1;
			}
		}

		Types::basic_type_e t = Types::getTypeFromString(&layoutType);

		Layout* layout = new Layout(t,dims.size(),extents);
		std::pair<std::string,Layout*> ly(layoutName,layout);
		layouts->insert(ly);
		INFO("Layout \"" << name << "\" now defined");
	}
*/
/*
	Layout* Configuration::getLayout(const char* name)
	{
		INFO("in getLayout");
		std::map<std::string,Layout*>::iterator it;
		it = layouts->find(std::string(name));
                
		if(it == layouts->end()) {
			return NULL;
		}
		
		return it->second;
	}

	Layout* Configuration::getVariableLayout(const char* varName)
	{
		INFO("in getVariableLayout");
		std::map<std::string,std::string>::iterator it;
		it = variableLayouts->find(std::string(varName));
		if(it == variableLayouts->end()) {
			return NULL;
		}
		
		return getLayout((it->second).c_str());
	}
*/
/*
	bool Configuration::getParameterType(const char* name, Types::basic_type_e* t)
	{
		INFO("searching parameter " << name);
		std::map<std::string,Parameter>::iterator i;
		i = parameters->find(std::string(name));
		if(i == parameters->end())
			return false;
		else
			*t = (i->second).type;
		return true;
	}
*/
//	
//	bool Configuration::getParameterValue(const char* name, void* v)
//	{
//		std::map<std::string,Parameter>::iterator it;
//		it = parameters->find(std::string(name));
//		if(it == parameters->end())
//			return false;
//		else
//		{
//			Types::basic_type_e t = (it->second).type;
//			switch(t) {
//			case(Types::SHORT) :
//				memcpy(v,(it->second).value.short_ptr,sizeof(short)); break;
//			case(Types::INT) :
//				memcpy(v,(it->second).value.int_ptr,sizeof(int)); break;
//			case(Types::LONG) :
//				memcpy(v,(it->second).value.long_ptr,sizeof(long)); break;
//			case(Types::FLOAT) : 
//				memcpy(v,(it->second).value.float_ptr,sizeof(float)); break;
//			case(Types::DOUBLE) :
//				memcpy(v,(it->second).value.double_ptr,sizeof(double)); break;
//			case(Types::CHAR) :
//				memcpy(v,(it->second).value.char_ptr,sizeof(char)); break;
//			default :
//				/* */ break;
//			/* TODO : all other types including string shouldn't be used */
//			}
//		}
//		return true;
//	}
//
//	int Configuration::getParameterString(const char* name, std::string* s)
//	{
//		/* TODO */
//		return 0;
//	}
/*
	void Configuration::setDataHierarchy(Group* g)
	{
		dataHierarchy = g;
	}
*/
/*	void Configuration::setSharedMemoryType(const char* str)
	{
		sharedMemoryModel = std::string(str);;
	}

	std::string Configuration::getSharedMemoryType()
	{
		return sharedMemoryModel;
	}
*/
	ParameterSet* Configuration::getParameterSet()
	{
		return &parameters;
	}

	MetadataManager* Configuration::getMetadataManager()
	{
		if(metadataManager == NULL)
		{
			metadataManager = new MetadataManager();
			fillMetadataManager();
		}
		return metadataManager;
	}

	void Configuration::fillMetadataManager()
	{
		// build all the layouts
		Model::data_mdl::layout_const_iterator l(baseModel->data().layout().begin());

		for(;l != baseModel->data().layout().end(); l++)
		{
			Types::basic_type_e type = Types::getTypeFromString(&(l->type()));
			
			if(type == Types::UNDEFINED_TYPE) {
				ERROR("Unkown type \"" << l->type() << "\" for layout \"" << l->name() << "\"");
				continue;
			}
			
			std::vector<int> dims;
			std::string name = (std::string)(l->name());
			std::string str = (std::string)(l->dimensions());
			std::string::const_iterator iter = str.begin();
			std::string::const_iterator end = str.end();
                
			bool r = phrase_parse(iter, end, *layoutInterp, boost::spirit::ascii::space, dims);
			if((!r) || (iter != str.end())) {
				ERROR("While parsing dimension descriptor for layout \"" << l->name() << "\"");
                        	continue;
			}
                 	Layout l(type,dims.size(),dims);	
			metadataManager->setLayout(name,l);
		}
		
		// build all the variables in root group
		Model::data_mdl::variable_const_iterator v(baseModel->data().variable().begin());
		for(; v != baseModel->data().variable().end(); v++)
		{
			std::string name = (std::string)(v->name());
			std::string layoutName = (std::string)(v->layout());
			Layout *l = metadataManager->getLayout(layoutName);
			if(l == NULL) {
				ERROR("No layout found for variable \"" << v->name() << "\"");
				continue;
			}
			Variable var(name,l);
			metadataManager->addVariableEntry(var);
		}		

		// build all variables in sub-groups
		Model::data_mdl::group_const_iterator g(baseModel->data().group().begin());
		for(; g != baseModel->data().group().end(); g++)
			readVariablesInSubGroup(&(*g),(std::string)(g->name()));
	}

	void Configuration::readVariablesInSubGroup(const Model::group_mdl *g, std::string groupName)
	{
		// first check if the group is enabled
		if(!(g->enabled())) return;
		// build recursively all variable in the subgroup
		Model::data_mdl::variable_const_iterator v(g->variable().begin());
                for(; v != g->variable().end(); v++)
                {
			std::string name = (std::string)(v->name());
			std::string layoutName = (std::string)(v->layout());
                        Layout *l = metadataManager->getLayout(layoutName);
                        if(l == NULL) {
                                ERROR("No layout found for variable \"" << (groupName + "/" + name) << "\"");
                                continue;
                        }
			std::string varName = groupName+"/"+name;
			Variable var(varName,l);
                        metadataManager->addVariableEntry(var);
                }

		// build recursively all the subgroups
		Model::data_mdl::group_const_iterator subg(g->group().begin());
		for(; subg != g->group().end(); subg++)
			readVariablesInSubGroup(&(*subg),groupName + "/" + (std::string)(subg->name()));
	}

	ActionsManager* Configuration::getActionsManager()
	{
		if(actionsManager == NULL)
		{
			actionsManager = new ActionsManager();
			fillActionsManager();
		}
		return actionsManager;
	}

	void Configuration::fillActionsManager()
	{
		Model::actions_mdl::event_const_iterator p(baseModel->actions().event().begin());
		for(; p < baseModel->actions().event().end(); p++) {
			std::string name = p->name();
			std::string fun = p->action();
			std::string file = p->library();
			actionsManager->loadActionFromPlugin(&name,&file,&fun);
		}
	}

	Configuration* Configuration::getInstance()
	{
		return m_instance;
	}

	void Configuration::initialize(std::auto_ptr<Model::simulation_mdl> mdl, std::string *cfgFile)
	{
		if(m_instance) {
			WARN("Configuration already initialized.");
			return;
		}
		m_instance = new Configuration(mdl,cfgFile);
	}
	
	void Configuration::finalize()
	{
		if(m_instance == NULL) {
			WARN("Configuration already finalized.");
			return;
		}
		delete m_instance;
	}

	std::string* Configuration::getMsgQueueName() const
	{
		return &(baseModel->architecture().queue().name());
	}

	size_t Configuration::getMsgQueueSize() const
	{
		return (size_t)(baseModel->architecture().queue().size());
	}

	std::string* Configuration::getSegmentName() const
	{
		return &(baseModel->architecture().buffer().name());
	}

	size_t Configuration::getSegmentSize() const
	{
		return (size_t)(baseModel->architecture().buffer().size());
	}

	int Configuration::getClientsPerNode() const
	{
		return baseModel->architecture().cores().clients().count();
	}
/*
	ActionsManager* Configuration::buildActionsManager()
	{
		// TODO
		return NULL;
	}
*/
}

