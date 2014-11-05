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

#include "util/Debug.hpp"
#include "data/CurveManager.hpp"

namespace damaris {

void CurveManager::Init(const model::Data& model)
{
	model::Data::curve_const_iterator m(model.curve().begin());
	for(; m != model.curve().end(); m++)
	{
		if(Create<Curve>(*m,(const std::string&)m->name()) != NULL) {
			DBG("Curve " << m->name() << " successfuly created");
		}
	}

	// build all variables in sub-groups
	model::Data::group_const_iterator g(model.group().begin());
	for(; g != model.group().end(); g++)
		ReadCurvesInSubGroup(*g,(std::string)(g->name()));
}

void CurveManager::ReadCurvesInSubGroup(const model::Group &g,
                        const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) return;
	// build recursively all variable in the subgroup
	model::Data::curve_const_iterator m(g.curve().begin());
	for(; m != g.curve().end(); m++)
	{
		std::string name = (std::string)(m->name());
		std::string varName = groupName+"/"+name;
		Create<Curve>(*m,varName);
	}

	// build recursively all the subgroups
	model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		ReadCurvesInSubGroup(*subg,groupName
				+ "/" + (std::string)(subg->name()));
}

}
