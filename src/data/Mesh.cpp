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
#include "data/Mesh.hpp"
#include "data/VariableManager.hpp"

namespace damaris {


std::shared_ptr<Variable> Mesh::FindVarInGroup(std::string& varName)
{
	std::shared_ptr<Variable>  ret_v = nullptr ;

	std::vector<std::string> groups;
	boost::split(groups, GetName(), boost::is_any_of("/"));

	while(not ret_v && groups.size() != 0) {

		groups.pop_back();
		std::string absoluteName;

		if(groups.size() != 0) {
			std::vector<std::string>::iterator it =
				groups.begin();
			absoluteName = *it;
			it++;
			for(; it != groups.end(); it++) {
				absoluteName += "/" + (*it);
			}
			absoluteName += "/" + varName;
		} else {
			absoluteName = varName;
		}
		ret_v = VariableManager::Search(absoluteName);
	}

	return ret_v ;
}


std::shared_ptr<Variable> Mesh::FindVar(std::string& varName)
{
	std::shared_ptr<Variable> vertid = nullptr ;


		bool vert_gid_IsAbsolute =
			(varName.find("/") != std::string::npos);

		if(vert_gid_IsAbsolute) {
			vertid = VariableManager::Search(varName);
		} else {
			vertid = FindVarInGroup(varName);
		}

		if(not vertid) {
			CFGERROR("Cannot find variable " << varName
				<< " to build mesh " << GetName());
		}

	 	return vertid ;


}

std::shared_ptr<Variable> Mesh::GetCoord(unsigned int n)
{
	if(coords_.size() == 0) { // first time access coordinates
		model::Mesh::coord_const_iterator 
			it(GetModel().coord().begin());
		for(; it != GetModel().coord().end(); it++) { 
			std::shared_ptr<Variable> v;
			std::string coordName = it->name();
			v = FindVar(coordName);
			coords_.push_back(v);
		}
	}

	if(n < coords_.size()) {
		return coords_[n];
	} else {
		return std::shared_ptr<Variable>();
	}
}

std::shared_ptr<Variable> Mesh::GetVertexGID()
{
	std::shared_ptr<Variable> vertid = nullptr ;
	std::string vert_gid_name = GetModel().vertex_global_id().get().name() ; // can also request offset()
	vertid = FindVar(vert_gid_name)  ;
 	return vertid ;
}

std::shared_ptr<Variable> Mesh::GetSectionVTKType()
{
	std::shared_ptr<Variable> vtktype = nullptr ;
	std::string sectn_type_name = GetModel().section_types().get().name() ;
	vtktype = FindVar(sectn_type_name)  ;
 	return vtktype ;
}

std::shared_ptr<Variable> Mesh::GetSectionSizes()
{
	std::shared_ptr<Variable> sctnsz = nullptr ;
	std::string sectn_size_name = GetModel().section_sizes().get().name() ;
	sctnsz = FindVar(sectn_size_name)  ;
 	return sctnsz ;
}

std::shared_ptr<Variable> Mesh::GetSectionVertexConnectivity()
{
	std::shared_ptr<Variable> sctn_connect = nullptr ;
	std::string sect_connect_name = GetModel().section_connectivity().get().name();
	sctn_connect = FindVar(sect_connect_name)  ;
 	return sctn_connect ;
}

}
