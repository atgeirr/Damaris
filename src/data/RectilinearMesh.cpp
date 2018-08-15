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

#ifdef HAVE_PARAVIEW_ENABLED
#include "damaris/paraview/ParaViewHeaders.hpp"
#endif

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


void RectilinearMesh::GetGridExtents(int source , int iteration , int block ,
                                     const std::shared_ptr<Variable>& coord ,
                                     int& lowExtent , int& highExtent)
{
    std::shared_ptr<Block> coordBlock = coord->GetBlock(source , iteration , block);

    if (coordBlock == nullptr) { // the coordinate has been written only in the first iteration
        std::shared_ptr<Block> coordBlock = coord->GetBlock(source , 0 , block);
    }

    auto type = coord->GetLayout()->GetType();
    size_t length = coord->GetLayout()->GetExtentAlong(0);

    switch(type) {
    case model::Type::short_:
        lowExtent = coordBlock->GetAt<short>(0);
        highExtent = coordBlock->GetAt<short>(length-1);
        break;
    case model::Type::int_:
    case model::Type::integer:
        lowExtent = coordBlock->GetAt<int>(0);
        highExtent = coordBlock->GetAt<int>(length-1);
        break;
    case model::Type::long_:
        lowExtent = coordBlock->GetAt<long>(0);
        highExtent = coordBlock->GetAt<long>(length-1);
        break;
    case model::Type::float_:
    case model::Type::real:
        lowExtent = coordBlock->GetAt<float>(0);
        highExtent = coordBlock->GetAt<float>(length-1);
        break;
    case model::Type::double_:
        lowExtent = coordBlock->GetAt<double>(0);
        highExtent = coordBlock->GetAt<double>(length-1);
        break;
    default:
        ERROR("Type is undefined for variable: " << coord->GetName());
    }
}

bool RectilinearMesh::GetGridInfo(int source , int iteration , int block ,
                                  std::shared_ptr<Variable>& vx ,
                                  std::shared_ptr<Variable>& vy ,
                                  std::shared_ptr<Variable>& vz ,
                                  int& lowX , int& highX , int& lowY ,
                                  int& highY , int& lowZ , int& highZ)
{
    unsigned int numCoord = GetNumCoord();
    if ((numCoord < 1) || (numCoord > 3)) {
        CFGERROR("The number of coordinate variabels is not correct!");
        return false;
    }

    vx = GetCoord(0);
    if (not vx) {
        CFGERROR("incorrect coordinate variable for dimention X!");
        return false;
    }
    else if (vx->GetLayout()->GetDimensions() != 1) {
        CFGERROR("incorrect variable for coordinate X of variable " << vx->GetName());
        return false;
    } else
        GetGridExtents(source , iteration , block , vx , lowX , highX);


    if (numCoord >= 2)
    {
        vy = GetCoord(1);
        if (not vy) {
            CFGERROR("incorrect coordinate variable for dimention Y!");
            return false;
        }
        else if (vy->GetLayout()->GetDimensions() != 1) {
            CFGERROR("incorrect variable for coordinate Y of variable " << vx->GetName());
            return false;
        } else
            GetGridExtents(source, iteration, block, vy, lowY, highY);
    }

    if (numCoord == 3)
    {
        vz = GetCoord(2);
        if (not vz) {
            CFGERROR("incorrect coordinate variable for dimention Z!");
            return false;
        }
        else if (vz->GetLayout()->GetDimensions() != 1) {
            CFGERROR("Incorrect variable for coordinate Z of variable " << vx->GetName());
            return false;
        } else
            GetGridExtents(source , iteration , block ,vz , lowZ , highZ);
    }

    return true;
}

template <typename T>
vtkDataArray* RectilinearMesh::CreateTypedDataArray(size_t length , T* pointer , size_t dataSize)
{
    vtkAOSDataArrayTemplate<T>* array = vtkAOSDataArrayTemplate<T>::New();
    array->SetNumberOfTuples(length);
    array->SetArray((T*)pointer , dataSize , 1);

    return array;
}

vtkDataArray* RectilinearMesh::CreateDataArray(int source , int iteration , int block ,
                                               std::shared_ptr<Variable> var )
{
     model::Type type = var->GetLayout()->GetType();
     size_t length = var->GetLayout()->GetExtentAlong(0);
     size_t layoutSize = var->GetLayout()->GetRequiredMemory();

     std::shared_ptr<Block> b = var->GetBlock(source , iteration , block);
     if (b == nullptr)
         b = var->GetBlock(source , 0 , block);

     ERROR("variable " << var->GetName() << " has " << var->CountLocalBlocks(iteration));

     void* pointer = b->GetDataSpace().GetData();

     switch(type) {
     case model::Type::short_:
         return CreateTypedDataArray<short>(length , (short*)pointer , layoutSize);
     case model::Type::int_:
     case model::Type::integer:
         return CreateTypedDataArray<int>(length , (int*)pointer , layoutSize);
     case model::Type::long_:
         return CreateTypedDataArray<long>(length , (long*)pointer , layoutSize);
     case model::Type::float_:
     case model::Type::real:
         return CreateTypedDataArray<float>(length , (float*)pointer , layoutSize);
     case model::Type::double_:
         return CreateTypedDataArray<double>(length , (double*)pointer , layoutSize);
     default:
         ERROR("Type is undefined for variable " << var->GetName());
     }

     return nullptr;
}

bool RectilinearMesh::SetGridCoord(int source , int iteration , int block ,
                                   int dim , std::shared_ptr<Variable> var ,
                                   std::shared_ptr<vtkRectilinearGrid> grid)
{
    if (var == nullptr) {
        ERROR("Cannot set grid coordinates for null variables. \n");
        return false;
    }

    // creating proper vtkDataArray based on its type
    vtkDataArray* coords = CreateDataArray(source , iteration , block , var );

    switch(dim) {
    case 0:
        grid->SetXCoordinates(coords);
        break;
    case 1:
        grid->SetYCoordinates(coords);
        break;
    case 2:
        grid->SetZCoordinates(coords);
        break;
    default:
        ERROR("Invalid value for the demention of the grid!");
        return false;
    }

    coords->Delete();

    return true;
}

std::shared_ptr<vtkDataSet> RectilinearMesh::GetVtkGrid(int source , int iteration , int block)
{
    /*if (vtkGrid_ != nullptr) // In future we should checck the change in the grid at run-time
        return vtkGrid_;*/

    std::shared_ptr<vtkRectilinearGrid> grid =
            std::shared_ptr<vtkRectilinearGrid>(vtkRectilinearGrid::New() , SelfDeleter<vtkRectilinearGrid>());


    int x1=0, x2=0, y1=0, y2=0, z1=0, z2=0; // mesh extents
    std::shared_ptr<Variable> vx = nullptr;
    std::shared_ptr<Variable> vy = nullptr;
    std::shared_ptr<Variable> vz = nullptr;

    if (not GetGridInfo(source , iteration , block , vx , vy , vz , x1 , x2 , y1 , y2 , z1 , z2)) {
        ERROR("Failed to get information related to the vtkGrid of: " << GetName());
        return nullptr;
    }

    grid->SetExtent(x1 , x2 , y1 , y2 , z1 , z2);
    ERROR("Setting Grid extents: Source: " << source << "   Extents: " << x1 << "," << x2 << "  " << y1 << "," << y2 << " " << z1 << "," << z2);

    //Setting the coordinates
    if (not SetGridCoord(source , iteration , block , 0 , vx , grid)) {
        ERROR("Setting the VTK Grid Coordinate for dimention 0 failed. ");
        return nullptr;
    }

    if (not SetGridCoord(source , iteration , block , 1 , vy , grid)) {
        ERROR("Setting the VTK Grid Coordinate for dimention 1 failed. ");
        return nullptr;
    }

    if (not SetGridCoord(source , iteration , block , 2 , vz , grid)) {
        ERROR("Setting the VTK Grid Coordinate for dimention 2 failed. ");
        return nullptr;
    }

    return grid;
}
#endif
}
