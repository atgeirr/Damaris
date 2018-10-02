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
 
#include "env/Environment.hpp"
#include "data/Variable.hpp"
#include "data/VariableManager.hpp"
#include "data/RectilinearMesh.hpp"



namespace damaris {

#ifdef HAVE_VISIT_ENABLED
bool RectilinearMesh::ExposeVisItMetaData(visit_handle md)
{
	visit_handle m1 = VISIT_INVALID_HANDLE;
	const model::Mesh& model = GetModel();
	
	if(VisIt_MeshMetaData_alloc(&m1) == VISIT_OKAY)
	{
		VisIt_MeshMetaData_setName(m1, GetName().c_str());
		VisIt_MeshMetaData_setMeshType(m1, VISIT_MESHTYPE_RECTILINEAR);
		//VisIt_MeshMetaData_setTopologicalDimension(m1, (int)model.topology());
		// topological dimension is not relevant in the context of a rectilinear mesh
		VisIt_MeshMetaData_setSpatialDimension(m1, (int)model.coord().size());

		// check that the coordinate exist
		std::shared_ptr<Variable> vx = GetCoord(0);
		std::shared_ptr<Variable> vy = GetCoord(1);
		std::shared_ptr<Variable> vz = GetCoord(2);
		if((not vx) || (not vy) || ((not vz) && GetNumCoord() == 3)) {
			VisIt_MeshMetaData_free(m1);
			return false;
		}

		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();
		int nbrClients = Environment::CountTotalClients();
		int numBlocks = nbrLocalBlocksPerClient*nbrClients;

		VisIt_MeshMetaData_setNumDomains(m1,numBlocks);
		
		model::Mesh::coord_const_iterator it(model.coord().begin());
		// the number of coordinates should be 2 or 3 
		// (this condition is checked by the xml loader)
		if(it->unit() != "#") 
			VisIt_MeshMetaData_setXUnits(m1,it->unit().c_str());
		if(it->label() != "#") 
			VisIt_MeshMetaData_setXLabel(m1,it->label().c_str());
		it++;

		if(it->unit() != "#") 
			VisIt_MeshMetaData_setYUnits(m1,it->unit().c_str());
		if(it->label() != "#") 
			VisIt_MeshMetaData_setYLabel(m1,it->label().c_str());
		it++;

		if(GetNumCoord() == 3) {
			if(it->unit() != "#") 
				VisIt_MeshMetaData_setZUnits(m1,it->unit().c_str());
			if(it->label() != "#") 
				VisIt_MeshMetaData_setZLabel(m1,it->label().c_str());
			it++;
		}

		VisIt_SimulationMetaData_addMesh(md, m1);
		return true;
	}
	return false;
}

bool RectilinearMesh::ExposeVisItData(visit_handle* h, 
	int source, int iteration, int block) 
{
	//const model::Mesh& model = GetModel();
	DBG("In RectilinearMesh::exposeVisItData");
	// Allocates the VisIt handle
	if(VisIt_RectilinearMesh_alloc(h) != VISIT_ERROR) {
		visit_handle hxc, hyc, hzc = VISIT_INVALID_HANDLE;
		std::shared_ptr<Variable>  vx, vy, vz;

		//model::Mesh::coord_const_iterator it(model.coord().begin());

		// Search for the X coordinate, checks that it has 1 dimenion
		vx = GetCoord(0);
			//VariableManager::Search(it->name());
		if(not vx) {
			//CFGERROR("Undefined coordinate \"" << it->name() 
			//		<<"\" for mesh \""
			//		<< GetName() << "\"");
			return false;
		}
		if(vx->GetLayout()->GetDimensions() != 1) {
			CFGERROR("Wrong number of dimensions for coordinate " 
				<< vx->GetName());
			return false;
		}
		//it++;

		// Search for the Y coordinate, checks that it has 1 dimension
		vy = GetCoord(1); 
			//VariableManager::Search(it->name());
		if(not vy) {
			//CFGERROR("Undefined coordinate \""
			//		<< it->name() <<"\" for mesh \""
			//		<< GetName() << "\"");
			return false;
		}
		if(vy->GetLayout()->GetDimensions() != 1) {
			CFGERROR("Wrong number of dimensions for coordinate " 
					<< vy->GetName());
			return false;
		}
		//it++;

		// Search for the Z coordinate if there is one, checks that it 
		// has 1 dimension
		if(GetNumCoord() == 3) {
			vz = GetCoord(2);
				//VariableManager::Search(it->name());
			if(not vz) {
				//ERROR("Undefined coordinate \""
				//	<< it->name() << "\" for mesh \""
				//	<< GetName() << "\"");
				return false;
			}
			if(vz->GetLayout()->GetDimensions() != 1) {
				CFGERROR("Wrong number of dimensions for coordinate " 
					<< vz->GetName());
				return false;
			}
		}

		// At this point, the 2 or 3 coordinate variables are found. 
		// Now accessing the data.

		// Accessing chunk for X coordinate
		std::shared_ptr<Block> c = vx->GetBlock(source,iteration,block);
		if(c) {
			if(VisIt_VariableData_alloc(&hxc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hxc);
			} else {
				ERROR("While allocating data handle");
				return false;
			}
		} else {
			VisIt_RectilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}

		// Accessing chunk for Y coordinate
		c = vy->GetBlock(source,iteration,block);
		if(c) {
			if(VisIt_VariableData_alloc(&hyc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hyc);
			} else {
				ERROR("While allocating data handle");
				VisIt_VariableData_free(hxc);
				VisIt_RectilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}
		} else {
			VisIt_VariableData_free(hxc);
			VisIt_RectilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}

		// Accessing chunk for Z coordinate we we need to
		if(GetNumCoord() == 3) {
			c = vz->GetBlock(source,iteration,block);
			if(c) {
				if(VisIt_VariableData_alloc(&hzc) == VISIT_OKAY) {
					c->FillVisItDataHandle(hzc);
				} else {
					ERROR("While allocating data handle");
					VisIt_VariableData_free(hxc);
					VisIt_VariableData_free(hyc);
					VisIt_RectilinearMesh_free(*h);
					*h = VISIT_INVALID_HANDLE;
					return false;
				}
			} else {
				ERROR("Data unavailable for coordinate \"" 
					<< vz->GetName() << "\"");
				VisIt_VariableData_free(hxc);
				VisIt_VariableData_free(hyc);
				VisIt_RectilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}	
		}

		// At this point, the 2 or 3 VisIt handle associated with the
		// coordinate variables have been created, we now have to link
		// the mesh data handle to the coordinate handles.

		if(GetNumCoord() == 2) {
			VisIt_RectilinearMesh_setCoordsXY(*h, hxc, hyc);
		} else if (GetNumCoord() == 3) {
			VisIt_RectilinearMesh_setCoordsXYZ(*h, hxc, hyc, hzc);
		} else {
			CFGERROR("How could you possibly reach this point???");
			VisIt_VariableData_free(hxc);
			VisIt_VariableData_free(hyc);
			VisIt_VariableData_free(hzc);
		}
	}

	return (*h != VISIT_INVALID_HANDLE);
}

#endif

#ifdef HAVE_PARAVIEW_ENABLED

bool RectilinearMesh::SetGridCoords(vtkDataSet* grid , int source , int iteration , int block ,
						   const std::shared_ptr<Variable>& vx ,
						   const std::shared_ptr<Variable>& vy ,
						   const std::shared_ptr<Variable>& vz)
{
    vtkRectilinearGrid* rectGrid = vtkRectilinearGrid::SafeDownCast(grid);

    if (vx != nullptr) {
        vtkDataArray* coordX = CreateCoordArray(source , iteration , block , vx );
        rectGrid->SetXCoordinates(coordX);
		coordX->Delete();    //-------------- ?????
    }

    if (vy != nullptr) {
        vtkDataArray* coordY = CreateCoordArray(source , iteration , block , vy );
        rectGrid->SetYCoordinates(coordY);
		coordY->Delete(); //------------------ ???????
    }

    if (vz != nullptr) {
		vtkDataArray* coordZ = CreateCoordArray(source , iteration , block , vz );
        rectGrid->SetZCoordinates(coordZ);
		coordZ->Delete(); //------------------ ???????
    }

    return true;
}


bool RectilinearMesh::SetGridExtents(vtkDataSet* grid , int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& var)
{
	int extents[6];
    vtkRectilinearGrid* rectGrid = vtkRectilinearGrid::SafeDownCast(grid);

	if (rectGrid == nullptr) {
		ERROR("Cannot downcast the parameter grid to vtkRectilinearGrid");
		return false;
	}

	std::shared_ptr<Block> b = GetCoordBlock(source , iteration , block , var);
	b->GetGridExtents(extents , var->IsZonal());
	rectGrid->SetExtent(extents);

	return true;
}

#endif

}
