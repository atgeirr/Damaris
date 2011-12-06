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
 * \file Configuration.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

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

	Configuration::Configuration()
	{
		initialized = false;
		actionsManager = NULL;
		metadataManager = NULL;
		environment = NULL;
	}

	Configuration::~Configuration()
	{
		ActionsManager::kill();
		MetadataManager::kill();
		Environment::kill();

		delete parameters;
		delete configFile;
		delete layoutInterp;
	}

	void Configuration::fillParameterSet()
	{
		Model::data_mdl::parameter_const_iterator p(baseModel->data().parameter().begin());
		for(; p < baseModel->data().parameter().end(); p++) {
			std::string name(p->name());
			if(p->type() == "short") {
				short value = boost::lexical_cast<short>(p->value());
				parameters->set<short>(name,value);
			} else
			if(p->type() == "int") {
				std::string name(p->name());
				int value = boost::lexical_cast<int>(p->value());
				parameters->set<int>(name,value);
			} else 
			if(p->type() == "long") {
				long value = boost::lexical_cast<long>(p->value());
				parameters->set<long>(name,value);
			} else
			if(p->type() == "float") {
				float value = boost::lexical_cast<float>(p->value());
				parameters->set<float>(name,value);
			} else
			if(p->type() == "double") {
				double value = boost::lexical_cast<double>(p->value());
				parameters->set<double>(name,value);
			} else
			if(p->type() == "char") {
				char value = boost::lexical_cast<char>(p->value());
				parameters->set<char>(name,value);
			} else
			if(p->type() == "string") {
				std::string value = boost::lexical_cast<std::string>(p->value());
				parameters->set<std::string>(name,value);
			} else
			{
				ERROR("Undefined type \"" << p->type() 
						<< "\" for parameter \""<< p->name() << "\"");
			}
		}
	}
	
	ParameterSet* Configuration::getParameterSet()
	{
		return parameters;
	}

	MetadataManager* Configuration::getMetadataManager()
	{
		if(metadataManager == NULL)
		{
			metadataManager = MetadataManager::getInstance();
			fillMetadataManager();
		}
		return metadataManager;
	}

	void Configuration::fillMetadataManager()
	{
		// build all the layouts
		DBG("Start filling the metadata manager");
		Model::data_mdl::layout_const_iterator l(baseModel->data().layout().begin());

		for(;l != baseModel->data().layout().end(); l++)
		{
			Types::basic_type_e type = Types::getTypeFromString(&(l->type()));
			Language::language_e language = 
				Language::getLanguageFromString(&(l->language()));

			if(type == Types::UNDEFINED_TYPE) {
				ERROR("Unknown type \"" << l->type() 
						<< "\" for layout \"" << l->name() << "\"");
				continue;
			}
			
			std::vector<int> dims;
			std::string name = (std::string)(l->name());
			std::string str = (std::string)(l->dimensions());
			std::string::const_iterator iter = str.begin();
			std::string::const_iterator end = str.end();
                
			bool r = phrase_parse(iter, end, *layoutInterp, 
					boost::spirit::ascii::space, dims);
			if((!r) || (iter != str.end())) {
				ERROR("While parsing dimension descriptor for layout \"" 
						<< l->name() << "\"");
                        	continue;
			}
			if(language == Language::LG_FORTRAN) {
				std::vector<int> rdims(dims.rbegin(),dims.rend());
				dims = rdims;
			}
                 	Layout l(name,type,dims.size(),dims);
			metadataManager->addLayout(name,l);
		}
		
		// build all the variables in root group
		Model::data_mdl::variable_const_iterator v(baseModel->data().variable().begin());
		for(; v != baseModel->data().variable().end(); v++)
		{
			std::string name = (std::string)(v->name());
			std::string layoutName = (std::string)(v->layout());
			metadataManager->addVariable(name,layoutName);
		}		

		// build all variables in sub-groups
		Model::data_mdl::group_const_iterator g(baseModel->data().group().begin());
		for(; g != baseModel->data().group().end(); g++)
			readVariablesInSubGroup(&(*g),(std::string)(g->name()));
	}

	void Configuration::readVariablesInSubGroup(const Model::group_mdl *g, 
			const std::string& groupName)
	{
		// first check if the group is enabled
		if(!(g->enabled())) return;
		// build recursively all variable in the subgroup
		Model::data_mdl::variable_const_iterator v(g->variable().begin());
                for(; v != g->variable().end(); v++)
                {
			std::string name = (std::string)(v->name());
			std::string layoutName = (std::string)(v->layout());
			std::string varName = groupName+"/"+name;
                        metadataManager->addVariable(varName,layoutName);
                }

		// build recursively all the subgroups
		Model::data_mdl::group_const_iterator subg(g->group().begin());
		for(; subg != g->group().end(); subg++)
			readVariablesInSubGroup(&(*subg),groupName 
							 + "/" + (std::string)(subg->name()));
	}

	ActionsManager* Configuration::getActionsManager()
	{
		if(actionsManager == NULL)
		{
			actionsManager = ActionsManager::getInstance();
			fillActionsManager();
		}
		return actionsManager;
	}

	void Configuration::fillActionsManager()
	{
		Model::actions_mdl::event_const_iterator e(baseModel->actions().event().begin());
		for(; e < baseModel->actions().event().end(); e++) {
			actionsManager->addDynamicAction(e->name(),e->library(),e->action(),e->scope());
		}
		Model::actions_mdl::script_const_iterator s(baseModel->actions().script().begin());
		for(; s < baseModel->actions().script().end(); s++) {
			actionsManager->addScriptAction(s->name(),s->file(),s->language(),s->scope());
		}
	}

	Environment* Configuration::getEnvironment()
	{
		return environment;
	}
	
	void Configuration::initialize(std::auto_ptr<Model::simulation_mdl> mdl, const std::string& cfgFile)
	{
		configFile = new std::string(cfgFile);
		baseModel = mdl;
		Model::simulation_mdl* tmpModel = baseModel.get();

		parameters = new ParameterSet();
		fillParameterSet();
		
		environment = Environment::getInstance();
		environment->initialize(tmpModel);

		layoutInterp = new Calc<std::string::const_iterator,ParameterSet>(*parameters);
		initialized = true;
	}
}

