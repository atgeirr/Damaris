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
#include "data/TypeLayout.hpp"
#include "data/BoxLayout.hpp"

namespace damaris {
	
void LayoutManager::AddSimpleTypes()
{
	Create<TypeLayout>(model::Type::short_,		"short");
	Create<TypeLayout>(model::Type::int_,		"int");
	Create<TypeLayout>(model::Type::integer,	"integer");
	Create<TypeLayout>(model::Type::long_,		"long");
	Create<TypeLayout>(model::Type::float_,		"float");
	Create<TypeLayout>(model::Type::real,		"real");
	Create<TypeLayout>(model::Type::double_,	"double");
	Create<TypeLayout>(model::Type::char_,		"char");
	Create<TypeLayout>(model::Type::character,	"character");
	Create<TypeLayout>(model::Type::label,		"label");
	Create<TypeLayout>(model::Type::string,		"string");
}

void LayoutManager::Init(const model::Data& mdl)
{
	DBG("Initializing LayoutManager");
	if(ParameterManager::IsEmpty() && mdl.parameter().size() > 0) {
		ParameterManager::Init(mdl);
	}
	
	// add simple types as layouts
	AddSimpleTypes();

	// add all layouts in the root group
	model::Data::layout_const_iterator l(mdl.layout().begin());
	for(;l != mdl.layout().end(); l++)
	{
		Create<BoxLayout>(*l);
	}

	// add all layouts in the sub-groups
	model::Data::group_const_iterator g(mdl.group().begin());
	for(; g != mdl.group().end(); g++)
		readLayoutsInSubGroup(*g,(std::string)(g->name()));
}

void LayoutManager::readLayoutsInSubGroup(const model::Group &g,
			const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) { 
		WARN("\"enabled\" attribute is deprecated.");
	}

	// build recursively all variable in the subgroup
	model::Data::layout_const_iterator l(g.layout().begin());
	for(; l != g.layout().end(); l++) {
		std::string name = (std::string)(l->name());
		std::string fullName = groupName+"/"+name;
		Create<BoxLayout>(*l,fullName);
	}

	// build recursively all the subgroups
	model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		readLayoutsInSubGroup(*subg,groupName
			+ "/" + (std::string)(subg->name()));
}

}
