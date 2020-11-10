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
#include "damaris/data/UnstructuredMesh.hpp"


namespace damaris {

#ifdef HAVE_PARAVIEW_ENABLED

bool UnstructuredMesh::GetGridVariables(std::shared_ptr<Variable>& coords_xyz,
										std::shared_ptr<Variable>& vert_gid,
										std::shared_ptr<Variable>& sect_vtk_type,
										std::shared_ptr<Variable>& sect_vtk_sizes,
										std::shared_ptr<Variable>& sect_vert_connect )
{
    unsigned int numCoord = GetNumCoord();
    if (numCoord != 1) {
        CFGERROR("Type UnstructuredMesh should have a single coord, with [][3] layout dimension");
        return false;
    }

    coords_xyz        = GetCoord(0);
    vert_gid          = GetVertexGID();
    sect_vtk_type     = GetVTKType();
    sect_vtk_sizes    = GetSectionSizes();
    sect_vert_connect = GetVertexConnectivity();

    return true;
}


void* UnstructuredMesh::GetCoordBuffer(int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& var)
{
	std::shared_ptr<Block> b = GetCoordBlock(source , iteration , block , var);
	return b->GetDataSpace().GetData();
}

std::shared_ptr<Block> UnstructuredMesh::GetCoordBlock(int source , int iteration , int block ,
													 const std::shared_ptr<Variable>& var)
{
	std::shared_ptr<Block> b = var->GetBlock(source , iteration , block);

	if (b == nullptr)  // no coord for this iteration
		b = var->GetBlock(source , 0 , block);

	if (b == nullptr) {
		ERROR("No coordinate blocks for variable " << var->GetName() << " in iteration " << iteration << std::endl );
		return nullptr;
	}

	return b;
}

template <typename T>
vtkDataArray* UnstructuredMesh::CreateTypedCoordArray(size_t length , T* pointer)
{
	vtkAOSDataArrayTemplate<T>* array = vtkAOSDataArrayTemplate<T>::New();
	array->SetNumberOfTuples(length);
	array->SetArray((T*)pointer , length , 1);

	return array;
}

vtkDataArray* UnstructuredMesh::CreateCoordArray(int source , int iteration , int block ,
											   const std::shared_ptr<Variable>& var )
{
     model::Type type = var->GetLayout()->GetType();
	 //size_t layoutSize = var->GetLayout()->GetNumberOfItems();

	 std::shared_ptr<Block>  b = GetCoordBlock(source ,  iteration , block , var);
	 void* pointer = b->GetDataSpace().GetData();
	 size_t blockSize = b->GetNumberOfItems();

     switch(type) {
     case model::Type::short_:
		 return CreateTypedCoordArray<short>(blockSize , (short*)pointer);
     case model::Type::int_:
     case model::Type::integer:
		 return CreateTypedCoordArray<int>(blockSize , (int*)pointer);
     case model::Type::long_:
		 return CreateTypedCoordArray<long>(blockSize , (long*)pointer);
     case model::Type::float_:
     case model::Type::real:
		 return CreateTypedCoordArray<float>(blockSize , (float*)pointer);
     case model::Type::double_:
		 return CreateTypedCoordArray<double>(blockSize , (double*)pointer);
     default:
         ERROR("Type is undefined for variable " << var->GetName());
     }

     return nullptr;
}

vtkDataSet* UnstructuredMesh::GetVtkGrid(int source , int iteration , int block ,
									   const std::shared_ptr<Variable>& var)
{
	std::shared_ptr<Variable> coords_xyz = nullptr;
	std::shared_ptr<Variable> vertex_gid = nullptr;
	std::shared_ptr<Variable> section_vtk_type = nullptr;
	std::shared_ptr<Variable> section_vtk_sizes = nullptr;
	std::shared_ptr<Variable> vertex_connectivity = nullptr;

	// Craeting the proper vtkGrid Object
	vtkDataSet* grid = CreateVtkGrid();

	// Getting the grid info
	if (not GetGridVariables(coords_xyz , vertex_gid , section_vtk_type, section_vtk_sizes, vertex_connectivity)) {
		ERROR("Failed to get variables related to the vtkGrid of: " << GetName());
		return nullptr;
	}

	// Setting the grid extents
	if (not SetGridExtents(grid , source ,  iteration ,  block , var)) {
		ERROR("Setting the grid extents failed.");
		return nullptr;
	}

	//Setting the coordinates
	if (not SetGridCoords(grid , source , iteration , block ,
						  vx , vy , vz)) {
		ERROR("Setting the curvilinear grid coordinates failed!");
		return nullptr;
	}

	return grid;
}
#endif


} // of namespace damaris
