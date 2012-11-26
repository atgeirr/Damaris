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
 * \file CurvilinearMesh.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "core/Environment.hpp"
#include "data/Variable.hpp"
#include "data/CurvilinearMesh.hpp"

namespace Damaris {
	
CurvilinearMesh::CurvilinearMesh(const Model::Mesh& mdl, const std::string& name)
: Mesh(mdl,name)
{ }

CurvilinearMesh* CurvilinearMesh::New(const Model::Mesh& mdl, const std::string& name)
{
	return new CurvilinearMesh(mdl,name);
}

#ifdef __ENABLE_VISIT
bool CurvilinearMesh::ExposeVisItMetaData(visit_handle md) const
{
	visit_handle m1 = VISIT_INVALID_HANDLE;
	if(VisIt_MeshMetaData_alloc(&m1) == VISIT_OKAY)
	{
		VisIt_MeshMetaData_setName(m1, GetName().c_str());
		VisIt_MeshMetaData_setMeshType(m1, VISIT_MESHTYPE_CURVILINEAR);
		VisIt_MeshMetaData_setTopologicalDimension(m1, (int)model.topology());
		VisIt_MeshMetaData_setSpatialDimension(m1, (int)model.coord().size());
	
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();
		int nbrClients = Environment::CountTotalClients();
		int numBlocks = nbrLocalBlocksPerClient*nbrClients;

		VisIt_MeshMetaData_setNumDomains(m1,numBlocks);


		Model::Mesh::coord_const_iterator it(model.coord().begin());
		// the number of coordinates should be 2 or 3 (this condition is checked by
		// the xml loader)
		if(it->unit() != "#") VisIt_MeshMetaData_setXUnits(m1,it->unit().c_str());
		if(it->label() != "#") VisIt_MeshMetaData_setXLabel(m1,it->label().c_str());
		it++;
		
		if(it->unit() != "#") VisIt_MeshMetaData_setYUnits(m1,it->unit().c_str());
		if(it->label() != "#") VisIt_MeshMetaData_setYLabel(m1,it->label().c_str());
		it++;
		
		if(model.coord().size() == 3) {
			if(it->unit() != "#") VisIt_MeshMetaData_setZUnits(m1,it->unit().c_str());
			if(it->label() != "#") VisIt_MeshMetaData_setZLabel(m1,it->label().c_str());
			it++;
		}

		VisIt_SimulationMetaData_addMesh(md, m1);
		return true;
	}
	return false;
}

bool CurvilinearMesh::ExposeVisItData(visit_handle* h, int source, int iteration, int block) const
{
	DBG("In CurvilinearMesh::ExposeVisItData");
	// Allocates the VisIt handle
	if(VisIt_CurvilinearMesh_alloc(h) != VISIT_ERROR) {
		visit_handle hxc, hyc, hzc = VISIT_INVALID_HANDLE;
		Variable *vx, *vy, *vz = NULL;

		Model::Mesh::coord_const_iterator it(model.coord().begin());

		// Search for the X coordinate, checks that it has 1 dimenion
		vx = Manager<Variable>::Search(it->name());
		if(vx == NULL) {
			CFGERROR("Undefined coordinate \""<< it->name() <<"\" for mesh \""
					<< GetName() << "\"");
			return false;
		}
		if(vx->GetLayout()->GetDimensions() != 3) {
			CFGERROR("Wrong number of dimensions for coordinate " << vx->GetName());
			return false;
		}
		it++;

		// Search for the Y coordinate, checks that it has 1 dimension
		vy = Manager<Variable>::Search(it->name());
		if(vy == NULL) {
			CFGERROR("Undefined coordinate \""<< it->name() <<"\" for mesh \""
					<< GetName() << "\"");
			return false;
		}
		if(vy->GetLayout()->GetDimensions() != 3) {
			CFGERROR("Wrong number of dimensions for coordinate " << vx->GetName());
			return false;
		}
		it++;

		// Search for the Z coordinate if there is one, checks that it has 1 dimension
		if(model.coord().size() == 3) {
			vz = Manager<Variable>::Search(it->name());
			if(vz == NULL) {
				ERROR("Undefined coordinate \""<< it->name() <<"\" for mesh \""
						<< GetName() << "\"");
				return false;
			}
			if(vz->GetLayout()->GetDimensions() != 3) {
				CFGERROR("Wrong number of dimensions for coordinate " << vx->GetName());
				return false;
			}
		}
		
		// At this point, the 2 or 3 coordinate variables are found. 
		// Now accessing the data.

		int cmesh_dims[3];

		// Accessing chunk for X coordinate
		Chunk* c = vx->GetChunk(source,iteration,block);
		if(c != NULL) {
			if(VisIt_VariableData_alloc(&hxc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hxc);
				cmesh_dims[0] = 1 + c->GetEndIndex(0) - c->GetStartIndex(0);
				cmesh_dims[1] = 1 + c->GetEndIndex(1) - c->GetStartIndex(1);
				cmesh_dims[2] = 1 + c->GetEndIndex(2) - c->GetStartIndex(2);
			} else {
				ERROR("While allocating data handle");
				VisIt_CurvilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}
		} else {
			VisIt_CurvilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}

		// Accessing chunk for Y coordinate
		c = vy->GetChunk(source,iteration,block);
		if(c != NULL) {
			if(VisIt_VariableData_alloc(&hyc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hyc);
				if(    (cmesh_dims[0] != 1 + c->GetEndIndex(0) - c->GetStartIndex(0))
					|| (cmesh_dims[1] != 1 + c->GetEndIndex(1) - c->GetStartIndex(1))
					|| (cmesh_dims[2] != 1 + c->GetEndIndex(2) - c->GetStartIndex(2))) {
					ERROR("Unmatching chunk sizes between coordinate variables");
					VisIt_VariableData_free(hxc);
					VisIt_VariableData_free(hyc);
					VisIt_CurvilinearMesh_free(*h);
					*h = VISIT_INVALID_HANDLE;
				}
			} else {
				ERROR("While allocating data handle");
				VisIt_VariableData_free(hxc);
				VisIt_CurvilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}
		} else {
			ERROR("Data unavailable for coordinate \"" << vy->GetName() << "\"");
			VisIt_VariableData_free(hxc);
			VisIt_CurvilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}
		
		// Accessing chunk for Z coordinate we we need to
		if(model.coord().size() == 3) {
			c = vz->GetChunk(source,iteration,block);
			if(c != NULL) {
				if(VisIt_VariableData_alloc(&hzc) == VISIT_OKAY) {
					c->FillVisItDataHandle(hzc);
					if(    (cmesh_dims[0] != 1 + c->GetEndIndex(0) - c->GetStartIndex(0))
						|| (cmesh_dims[1] != 1 + c->GetEndIndex(1) - c->GetStartIndex(1))
						|| (cmesh_dims[2] != 1 + c->GetEndIndex(2) - c->GetStartIndex(2))) {
						ERROR("Unmatching chunk sizes between coordinate variables");
						VisIt_VariableData_free(hxc);
						VisIt_VariableData_free(hyc);
						VisIt_VariableData_free(hzc);
						VisIt_CurvilinearMesh_free(*h);
						*h = VISIT_INVALID_HANDLE;
					}
				} else {
					ERROR("While allocating data handle");
					VisIt_VariableData_free(hxc);
					VisIt_VariableData_free(hyc);
					VisIt_CurvilinearMesh_free(*h);
					*h = VISIT_INVALID_HANDLE;
					return false;
				}
			} else {
				VisIt_VariableData_free(hxc);
				VisIt_VariableData_free(hyc);
				VisIt_CurvilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}	
		}

		// At this point, the 2 or 3 VisIt handle associated with the
		// coordinate variables have been created, we now have to link
		// the mesh data handle to the coordinate handles.
		
		// for some unknown reason, the mesh dimensions must be interchanged
		if(model.coord().size() == 2) {
			int t = cmesh_dims[0];
			cmesh_dims[0] = cmesh_dims[1];
			cmesh_dims[1] = t;
			VisIt_CurvilinearMesh_setCoordsXY(*h, cmesh_dims, hxc, hyc);
		} else if (model.coord().size() == 3) {
			int t = cmesh_dims[0];
			cmesh_dims[0] = cmesh_dims[2];
			cmesh_dims[2] = t;
			VisIt_CurvilinearMesh_setCoordsXYZ(*h, cmesh_dims, hxc, hyc, hzc);
		} else {
			CFGERROR("How could you possibly reach this point???");
			VisIt_VariableData_free(hxc);
			VisIt_VariableData_free(hyc);
			VisIt_VariableData_free(hzc);
			VisIt_CurvilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
		}
	}

	return (*h != VISIT_INVALID_HANDLE);
}

#endif

}
