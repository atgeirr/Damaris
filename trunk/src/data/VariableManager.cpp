/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include "util/Debug.hpp"
#include "data/LayoutManager.hpp"
#include "data/VariableManager.hpp"

namespace damaris {

void VariableManager::Init(const model::Data& mdl)
{
	if(LayoutManager::IsEmpty()) {
		LayoutManager::Init(mdl);
	}
	// build all the variables in root group
	model::Data::variable_const_iterator v(mdl.variable().begin());
	for(; v != mdl.variable().end(); v++)
	{
		Create<Variable>(*v);
	}

	// build all variables in sub-groups
	model::Data::group_const_iterator g(mdl.group().begin());
	for(; g != mdl.group().end(); g++)
		readVariablesInSubGroup(*g,(std::string)(g->name()));
}

void VariableManager::readVariablesInSubGroup(const model::Group &g,
                        const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) {
		WARN("\"enabled\" attribute is deprecated.");
	}
	// build recursively all variable in the subgroup
	model::Data::variable_const_iterator v(g.variable().begin());
	for(; v != g.variable().end(); v++)
	{
		std::string name = (std::string)(v->name());
		std::string varName = groupName+"/"+name;
		Create<Variable>(*v,varName);
	}

	// build recursively all the subgroups
	model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		readVariablesInSubGroup(*subg,groupName
				+ "/" + (std::string)(subg->name()));
}

}
