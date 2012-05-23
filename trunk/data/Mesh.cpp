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
 * \file Mesh.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "data/Mesh.hpp"
#include "data/RectilinearMesh.hpp"
#include "core/Debug.hpp"

namespace Damaris {

Mesh::Mesh(const Model::Mesh& mdl, const std::string &n)
: Configurable<Model::Mesh>(mdl)
{
	name = n;
}

Mesh* Mesh::New(const Model::Mesh& mdl, const std::string& name)
{
	switch(mdl.type()) {
		case Model::MeshType::rectilinear :
			return RectilinearMesh::New(mdl,name);
        case Model::MeshType::curvilinear :
        case Model::MeshType::unstructured :
        case Model::MeshType::point :
        case Model::MeshType::csg :
        case Model::MeshType::amr :
        case Model::MeshType::unknown :
		default: break;
	}
	ERROR("Mesh type " << mdl.type() << " is not implemented.");
	return NULL;
}

}
