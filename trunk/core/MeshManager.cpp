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
 * \file VariableManager.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Debug.hpp"
#include "core/MeshManager.hpp"

namespace Damaris {

void MeshManager::Init(const Model::Data& model)
{
	Model::Data::mesh_const_iterator m(model.mesh().begin());
	for(; m != model.mesh().end(); m++)
	{
		Create<Mesh>(*m,(const std::string&)m->name());
	}

	// build all variables in sub-groups
	Model::Data::group_const_iterator g(model.group().begin());
	for(; g != model.group().end(); g++)
		readMeshesInSubGroup(*g,(std::string)(g->name()));
}

void MeshManager::readMeshesInSubGroup(const Model::Group &g,
                        const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) return;
	// build recursively all variable in the subgroup
	Model::Data::mesh_const_iterator m(g.mesh().begin());
	for(; m != g.mesh().end(); m++)
	{
		std::string name = (std::string)(m->name());
		std::string varName = groupName+"/"+name;
		Create<Mesh>(*m,varName);
	}

	// build recursively all the subgroups
	Model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		readMeshesInSubGroup(*subg,groupName
				+ "/" + (std::string)(subg->name()));
}

}
