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
#include "core/Debug.hpp"
#include "core/VariableManager.hpp"

namespace Damaris {

void VariableManager::Init(const Model::Data& model)
{
	// build all the variables in root group
	Model::Data::variable_const_iterator v(model.variable().begin());
	for(; v != model.variable().end(); v++)
	{
		Create<Variable>(*v,(const std::string&)v->name());
	}

	// build all variables in sub-groups
	Model::Data::group_const_iterator g(model.group().begin());
	for(; g != model.group().end(); g++)
		readVariablesInSubGroup(*g,(std::string)(g->name()));
}

void VariableManager::readVariablesInSubGroup(const Model::Group &g,
                        const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) return;
	// build recursively all variable in the subgroup
	Model::Data::variable_const_iterator v(g.variable().begin());
	for(; v != g.variable().end(); v++)
	{
		std::string name = (std::string)(v->name());
		std::string varName = groupName+"/"+name;
		Create<Variable>(*v,varName);
	}

	// build recursively all the subgroups
	Model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		readVariablesInSubGroup(*subg,groupName
				+ "/" + (std::string)(subg->name()));
}

}
