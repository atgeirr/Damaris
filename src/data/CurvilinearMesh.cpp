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
#include "data/CurvilinearMesh.hpp"

namespace damaris {

#ifdef HAVE_VISIT_ENABLED 
bool CurvilinearMesh::ExposeVisItMetaData(visit_handle md) 
{
	const model::Mesh& model = GetModel();
	visit_handle m1 = VISIT_INVALID_HANDLE;
	if(VisIt_MeshMetaData_alloc(&m1) == VISIT_OKAY)
	{
		VisIt_MeshMetaData_setName(m1, GetName().c_str());
		VisIt_MeshMetaData_setMeshType(m1, VISIT_MESHTYPE_CURVILINEAR);
		VisIt_MeshMetaData_setTopologicalDimension(m1, (int)model.topology());
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
		// the number of coordinates should be 2 or 3 (this condition 
		// is checked by the xml loader)
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

bool CurvilinearMesh::ExposeVisItData(visit_handle* h, 
	int source, int iteration, int block) 
{
	DBG("In CurvilinearMesh::ExposeVisItData");
	// Allocates the VisIt handle
	if(VisIt_CurvilinearMesh_alloc(h) != VISIT_ERROR) {
		visit_handle hxc, hyc, hzc = VISIT_INVALID_HANDLE;
		std::shared_ptr<Variable> vx, vy, vz;

		// Search for the X coordinate, checks that it has 1 dimenion
		vx = GetCoord(0); 
		if(not vx) {
			return false;
		}
		if(vx->GetLayout()->GetDimensions() != GetTopology()) {
			CFGERROR("Wrong number of dimensions for coordinate " 
				<< vx->GetName());
			return false;
		}

		// Search for the Y coordinate, checks that it has 1 dimension
		vy = GetCoord(1);
		if(not vy) {
			return false;
		}
		if(vy->GetLayout()->GetDimensions() != GetTopology()) {
			CFGERROR("Wrong number of dimensions for coordinate " 
				<< vy->GetName());
			return false;
		}

		// Search for the Z coordinate if there is one, 
		if(GetNumCoord() == 3) {
			vz = GetCoord(2);
			if(not vz) {
				CFGERROR("Z dimension not found");
				return false;
			}
			if(vz->GetLayout()->GetDimensions() != GetTopology()) {
				CFGERROR("Wrong number of dimensions for coordinate " 
					<< vz->GetName());
				return false;
			}
		}
		
		// At this point, the 2 or 3 coordinate variables are found. 
		// Now accessing the data.

		int cmesh_dims[3];

		// Accessing chunk for X coordinate
		std::shared_ptr<Block> c = vx->GetBlock(source,iteration,block);
		if(c) {
			if(VisIt_VariableData_alloc(&hxc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hxc);
				cmesh_dims[0] = 
					1 + c->GetEndIndex(0) - c->GetStartIndex(0);
				cmesh_dims[1] = 
					1 + c->GetEndIndex(1) - c->GetStartIndex(1);
				if(GetTopology() == 3)
					cmesh_dims[2] = 
						1 + c->GetEndIndex(2) - c->GetStartIndex(2);
			} else {
				ERROR("While allocating data handle");
				VisIt_CurvilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}
		} else {
			VisIt_CurvilinearMesh_free(*h);
			*h = VISIT_INVALID_HANDLE;
			ERROR("Data unavailable for coordinate \"" << vx->GetName() << "\"");
			return false;
		}

		// Accessing chunk for Y coordinate
		c = vy->GetBlock(source,iteration,block);
		if(c) {
			if(VisIt_VariableData_alloc(&hyc) == VISIT_OKAY) {
				c->FillVisItDataHandle(hyc);
				if((cmesh_dims[0] != 1 + c->GetEndIndex(0) - c->GetStartIndex(0))
				|| (cmesh_dims[1] != 1 + c->GetEndIndex(1) - c->GetStartIndex(1))
				|| ( GetTopology() == 3 && (cmesh_dims[2] != 1 + c->GetEndIndex(2) - c->GetStartIndex(2)))) {
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
		if(GetNumCoord() == 3) {
			c = vz->GetBlock(source,iteration,block);
			if(c != NULL) {
				if(VisIt_VariableData_alloc(&hzc) == VISIT_OKAY) {
					c->FillVisItDataHandle(hzc);
					if((cmesh_dims[0] != 1 + c->GetEndIndex(0) - c->GetStartIndex(0))
					|| (cmesh_dims[1] != 1 + c->GetEndIndex(1) - c->GetStartIndex(1))
					|| ( GetTopology() == 3 && (cmesh_dims[2] != 1 + c->GetEndIndex(2) - c->GetStartIndex(2)))) {
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
				ERROR("Data unavailable for coordinate \"" << vz->GetName() << "\"");
				VisIt_VariableData_free(hxc);
				VisIt_VariableData_free(hyc);
				VisIt_CurvilinearMesh_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}	
		}

		// At this point, the 2 or 3 VisIt handles associated with the
		// coordinate variables have been created, we now have to link
		// the mesh data handle to the coordinate handles.
		
		// for some unknown reason, 
		// the mesh dimensions must be interchanged...
		if(GetTopology() == 2) {
			int t = cmesh_dims[0];
			cmesh_dims[0] = cmesh_dims[1];
			cmesh_dims[1] = t;
			cmesh_dims[2] = 1;
		} else if (GetTopology() == 3) {
			int t = cmesh_dims[0];
			cmesh_dims[0] = cmesh_dims[2];
			cmesh_dims[2] = t;
		}

		if(GetNumCoord() == 2) {
			VisIt_CurvilinearMesh_setCoordsXY(*h, cmesh_dims, hxc, hyc);
		} else if(GetNumCoord() == 3) {
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


#ifdef HAVE_PARAVIEW_ENABLED

template<typename T>
vtkDataArray* CurvilinearMesh::GetPointsArray(int source , int iteration , int block ,
											  std::shared_ptr<Variable> vx ,
											  std::shared_ptr<Variable> vy ,
											  std::shared_ptr<Variable> vz )
{
	int dimension = GetNumCoord();
	int gridSize = 0;
	vtkSOADataArrayTemplate<T>* soaArray = vtkSOADataArrayTemplate<T>::New();
	soaArray->SetNumberOfComponents(dimension);
	vtkDataArray* coordX = nullptr;;

	if (vx != nullptr) {
		vtkDataArray* coordX = CreateCoordArray(source , iteration , block , vx );
		gridSize = coordX->GetNumberOfTuples();
		soaArray->SetNumberOfTuples(gridSize);
	}
	else {
		ERROR("Variable Vx for the mesh " << GetName() << " is null");
		return nullptr;
	}

	if (dimension >= 1) {
		T* bufferX = (T*)GetCoordBuffer(source , iteration , block , vx);
		soaArray->SetArray(0 , bufferX , gridSize , false , true);
	}

	if (dimension >= 2) {
		T* bufferY = (T*)GetCoordBuffer(source , iteration , block , vy);
		soaArray->SetArray(1 , bufferY , gridSize , false , true);
	}

	if (dimension == 3) {
		T* bufferZ = (T*)GetCoordBuffer(source , iteration , block , vz);
		soaArray->SetArray(2 , bufferZ , gridSize , false , true);
	}

	return soaArray;
}


bool CurvilinearMesh::SetGridCoords(vtkDataSet* grid , int source , int iteration , int block ,
                           std::shared_ptr<Variable> vx ,
                           std::shared_ptr<Variable> vy ,
						   std::shared_ptr<Variable> vz )
{
	vtkDataArray* points = nullptr;
	auto meshType = vx->GetLayout()->GetType();
	vtkStructuredGrid* curvGrid = vtkStructuredGrid::SafeDownCast(grid);

	switch(meshType) {
	case model::Type::short_:
		points = GetPointsArray<short>(source , iteration , block , vx , vy , vz);
		break;
	case model::Type::int_:
	case model::Type::integer:
		points = GetPointsArray<int>(source , iteration , block , vx , vy , vz);
		break;
	case model::Type::long_:
		points = GetPointsArray<long>(source , iteration , block , vx , vy , vz);
		break;
	case model::Type::float_:
	case model::Type::real:
		points = GetPointsArray<float>(source , iteration , block , vx , vy , vz);
		break;
	case model::Type::double_:
		points = GetPointsArray<double>(source , iteration , block , vx , vy , vz);
		break;
	default:
		ERROR("Type is undefined for mesh with its vx variable as: " << vx->GetName());
		return false;
	}

	vtkPoints* pointArray = vtkPoints::New();
	pointArray->SetData(points);
	curvGrid->SetPoints(pointArray);

	//points->Delete();
	//pointArray->Delete();

	return true;
}

bool CurvilinearMesh::SetGridExtents(vtkDataSet* grid , std::shared_ptr<Variable> var,
									 int source , int iteration , int block)
{
	int extents[6];
	vtkStructuredGrid* curvGrid = vtkStructuredGrid::SafeDownCast(grid);

	if (curvGrid != nullptr) {
		std::shared_ptr<Block> b = GetCoordBlock(source , iteration , block , var);

		b->GetExtents(extents);
		curvGrid->SetExtent(extents);

		return true;
	}

	ERROR("Cannot downcast the parameter grid to vtkCurvilinearGrid");
	return false;
}

#endif
}
