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
 * \file RectilinearMesh.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "data/Variable.hpp"
#include "data/RectilinearMesh.hpp"

namespace Damaris {
namespace Viz {
	
RectilinearMesh::RectilinearMesh(const Model::Mesh& mdl)
: Mesh(mdl)
{ }

#ifdef __ENABLE_VISIT
bool RectilinearMesh::exposeVisItMetaData(visit_handle md) const
{
	visit_handle m1 = VISIT_INVALID_HANDLE;
	if(VisIt_MeshMetaData_alloc(&m1) == VISIT_OKAY)
	{
		VisIt_MeshMetaData_setName(m1, getName().c_str());
		VisIt_MeshMetaData_setMeshType(m1, VISIT_MESHTYPE_RECTILINEAR);
		VisIt_MeshMetaData_setTopologicalDimension(m1, (int)model.topology());
		VisIt_MeshMetaData_setSpatialDimension(m1, (int)model.coords().size());

		Model::Mesh::coords_const_iterator it(model.coords().begin());
		if(model.coords().size() >= 1) {
			if(it->unit() != "#") VisIt_MeshMetaData_setXUnits(m1,it->unit().c_str());
			if(it->label() != "#") VisIt_MeshMetaData_setXLabel(m1,it->label().c_str());
			it++;
		}
		if(model.coords().size() >= 2) {
			if(it->unit() != "#") VisIt_MeshMetaData_setYUnits(m1,it->unit().c_str());
            if(it->label() != "#") VisIt_MeshMetaData_setYLabel(m1,it->label().c_str());
			it++;
		}
		if(model.coords().size() >= 3) {
			if(it->unit() != "#") VisIt_MeshMetaData_setZUnits(m1,it->unit().c_str());
            if(it->label() != "#") VisIt_MeshMetaData_setZLabel(m1,it->label().c_str());
			it++;
		}

		VisIt_SimulationMetaData_addMesh(md, m1);
		return true;
	}
	return false;
}

bool Mesh::exposeVisItData() const
{
	visit_handle h = VISIT_INVALID_HANDLE;
	if(VisIt_RectilinearMesh_alloc(&h) != VISIT_ERROR) {
		visit_handle hxc, hyc, hzc = VISIT_INVALID_HANDLE;
		Variable *vx, *vy, *vz;

		{
			Model::Mesh::coords_const_iterator it(model.coords().begin());
			vx = Manager<Variable,Model::Variable>::Search(it->name());
			hxc = vx->getVisItHandle();
			it++;
			vy = Manager<Variable,Model::Variable>::Search(it->name());
			hyc = vy->getVisItHandle();
			it++;
			if(model.coords().size() == 3) {
				vz = Manager<Variable,Model::Variable>::Search(it->name());
				hzc = vz->getVisItHandle();
			}
		}

		if(model.coords().size() == 2) {
			VisIt_RectilinearMesh_setCoordsXY(h, hxc, hyc);
	 	} else {
			VisIt_RectilinearMesh_setCoordsXYZ(h, hxc, hyc, hzc);
		}
	}
	return (h != VISIT_INVALID_HANDLE);
}

#endif
}
}
