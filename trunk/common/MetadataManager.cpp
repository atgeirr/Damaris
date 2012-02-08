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
 * \file MetadataManager.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "common/Debug.hpp"
#include "common/MetadataManager.hpp"

namespace Damaris {

MetadataManager::MetadataManager(Model::DataModel* mdl, Environment* env)
: Configurable<MetadataManager,Model::DataModel>(mdl)
{
	environment = env;
	init();
}

void MetadataManager::init()
{
	initParameters();
	initLayouts();
	initVariables();	
}

void MetadataManager::initParameters()
{
	parameters = new ParameterSet(model);
	layoutInterp = new Calc<std::string::const_iterator,ParameterSet>(*parameters);
}

void MetadataManager::initLayouts()
{
	if(model == NULL) return;

	Model::DataModel::layout_const_iterator l(model->layout().begin());
	for(;l != model->layout().end(); l++)
	{
		Types::basic_type_e type = Types::getTypeFromString(l->type());
		Language::language_e language =
			Language::getLanguageFromString(&(l->language()));
		if(language == Language::LG_UNKNOWN)
			language = environment->getDefaultLanguage();
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
		bool r = boost::spirit::qi::phrase_parse(iter, end, *layoutInterp,
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
		addLayout(name,l);
	}
}

void MetadataManager::initVariables()
{
	variables = new VariableSet();

	if(model == NULL) return;
	// build all the variables in root group
	Model::DataModel::variable_const_iterator v(model->variable().begin());
	for(; v != model->variable().end(); v++)
	{
		std::string name = (std::string)(v->name());
		std::string layoutName = (std::string)(v->layout());
		std::string desc(*v);
		std::string unit = (std::string)(v->unit());
		addVariable(name,layoutName,desc,unit);
	}

	// build all variables in sub-groups
	Model::DataModel::group_const_iterator g(model->group().begin());
	for(; g != model->group().end(); g++)
		readVariablesInSubGroup(&(*g),(std::string)(g->name()));
}

void MetadataManager::readVariablesInSubGroup(const Model::GroupModel *g,
                        const std::string& groupName)
{
	if(g == NULL) return;
	// first check if the group is enabled
	if(!(g->enabled())) return;
	// build recursively all variable in the subgroup
	Model::DataModel::variable_const_iterator v(g->variable().begin());
	for(; v != g->variable().end(); v++)
	{
		std::string name = (std::string)(v->name());
		std::string layoutName = (std::string)(v->layout());
		std::string varName = groupName+"/"+name;
		std::string desc(*v);
		std::string unit = (std::string)(v->unit());
                addVariable(name,layoutName,desc,unit);
		addVariable(varName,layoutName);
	}

	// build recursively all the subgroups
	Model::DataModel::group_const_iterator subg(g->group().begin());
	for(; subg != g->group().end(); subg++)
		readVariablesInSubGroup(&(*subg),groupName
				+ "/" + (std::string)(subg->name()));
}

bool MetadataManager::addVariable(const std::string &varname, const std::string & layoutname)
{
	return addVariable(varname,layoutname,"","");
}

bool MetadataManager::addVariable(const std::string & varname, const std::string & layoutname,
				  const std::string & description, const std::string & unit)
{
	VariableSet::index<by_name>::type::iterator it = 
		variables->get<by_name>().find(varname);
	
	if(it != variables->get<by_name>().end()) {
		WARN("Inserting a variable with a name"
				<<" identical to a previously defined variable");
		return false;
	}

	// check that the layout exists
	Layout* l = getLayout(layoutname);
	if(l == NULL) {
		ERROR("Undefined layout \"" << layoutname << "\" for variable \""
			<< varname << "\"");
		return false;
	}

	// allocate the variable
	int id = variables->size();
	Variable* v = new Variable(id,varname,l);
	v->setDescription(description);
	v->setUnit(unit);

	variables->insert(boost::shared_ptr<Variable>(v));
	DBG("Variable \"" << varname << "\" now defined in the metadata manager");
	return true;
}

Variable* MetadataManager::getVariable(const std::string &name)
{
	VariableSet::index<by_name>::type::iterator it = 
		variables->get<by_name>().find(name);
	if(it == variables->get<by_name>().end()) {
		return NULL;
	}
	return it->get();
}

Variable* MetadataManager::getVariable(int id)
{
	VariableSet::index<by_id>::type::iterator it =
		variables->get<by_id>().find(id);
	if(it == variables->get<by_id>().end()) {
		return NULL;
	}
	return it->get();
}

bool MetadataManager::addLayout(const std::string& lname, Layout &l)
{
	if(layouts.find(lname) != layouts.end())
	{
		ERROR("Trying to define two layouts with the same name \"" << lname << "\"");
		return false;
	}
	layouts.insert(std::pair<std::string,Layout>(lname,l));
	DBG("Layout \"" << lname << "\" is now defined in the metadata manager");
	return true;
}

Layout* MetadataManager::getLayout(const std::string& lname)
{
	std::map<std::string,Layout>::iterator it = layouts.find(lname);
	if(it == layouts.end()) return NULL;
	return &(it->second);
}

void MetadataManager::listVariables(std::ostream &out)
{
	VariableSet::index<by_name>::type::iterator it = variables->get<by_name>().begin();
	const VariableSet::index<by_name>::type::iterator &end = variables->get<by_name>().end();
	INFO("Listing all the variables defined in the Metadata Manager:");
	while(it != end) {
		out << it->get()->getID() << "\t:\t" << it->get()->getName() << std::endl;
		it++;
	}
}

MetadataManager::~MetadataManager()
{
	delete variables;
	delete parameters;
	delete layoutInterp;
}

}
