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
#include "viz/Mesh.hpp"
#include "viz/RectilinearMesh.hpp"
#include "common/Debug.hpp"

namespace Damaris {
namespace Viz {
	
Mesh::Mesh(const Model::Mesh& mdl)
: Configurable<Model::Mesh>(mdl)
{ }

Mesh* Mesh::New(const Model::Mesh& mdl)
{
	switch(mdl.type()) {
		case Model::MeshType::rectilinear :
			return new RectilinearMesh(mdl);
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
}
