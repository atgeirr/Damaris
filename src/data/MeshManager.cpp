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
#include "data/VariableManager.hpp"
#include "data/MeshManager.hpp"

#include "data/RectilinearMesh.hpp"
#include "data/CurvilinearMesh.hpp"
#include "data/PointMesh.hpp"
#include "data/UnstructuredMesh.hpp"

namespace damaris {

void MeshManager::Init(const model::Data& mdl)
{
	if(VariableManager::IsEmpty()) {
		VariableManager::Init(mdl);
	}
	// build all the variables in root group
	model::Data::mesh_const_iterator m(mdl.mesh().begin());
	for(; m != mdl.mesh().end(); m++)
	{
		switch(m->type()) {
		case model::MeshType::rectilinear :
			Create<RectilinearMesh>(*m); break;
		case model::MeshType::curvilinear :
			Create<CurvilinearMesh>(*m); break;
		case model::MeshType::point :
			Create<PointMesh>(*m); break;
		case model::MeshType::unstructured :
			Create<UnstructuredMesh>(*m); break;
		case model::MeshType::csg :
		case model::MeshType::amr :
		case model::MeshType::unknown :
		default: break;
		}
	}

	// build all meshes in sub-groups
	model::Data::group_const_iterator g(mdl.group().begin());
	for(; g != mdl.group().end(); g++)
		ReadMeshesInSubGroup(*g,(std::string)(g->name()));
	
}

void MeshManager::ReadMeshesInSubGroup(const model::Group &g,
                        const std::string& groupName)
{
	// first check if the group is enabled
	if(!(g.enabled())) return;
	// build recursively all variable in the subgroup
	model::Data::mesh_const_iterator m(g.mesh().begin());
	for(; m != g.mesh().end(); m++)
	{
		std::string name = (std::string)(m->name());
		std::string meshName = groupName+"/"+name;
		switch(m->type()) {
		case model::MeshType::rectilinear :
			Create<RectilinearMesh>(*m,meshName); break;
		case model::MeshType::curvilinear :
			Create<CurvilinearMesh>(*m,meshName); break;
		case model::MeshType::point :
			Create<PointMesh>(*m,meshName); break;
		case model::MeshType::unstructured :
			Create<UnstructuredMesh>(*m); break;
		case model::MeshType::csg :
		case model::MeshType::amr :
		case model::MeshType::unknown :
		default: break;
		}
	}

	// build recursively all the subgroups
	model::Data::group_const_iterator subg(g.group().begin());
	for(; subg != g.group().end(); subg++)
		ReadMeshesInSubGroup(*subg,groupName
				+ "/" + (std::string)(subg->name()));
}

}
