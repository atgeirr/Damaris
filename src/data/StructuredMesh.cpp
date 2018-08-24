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
#include "damaris/data/StructuredMesh.hpp"


namespace damaris {

#ifdef HAVE_PARAVIEW_ENABLED

bool StructuredMesh::GetGridVariables(std::shared_ptr<Variable>& vx ,
									  std::shared_ptr<Variable>& vy ,
									  std::shared_ptr<Variable>& vz )
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

	if (numCoord >= 2) {
        vy = GetCoord(1);
        if (not vy) {
            CFGERROR("incorrect coordinate variable for dimention Y!");
            return false;
        }
    }

	if (numCoord == 3) {
        vz = GetCoord(2);
        if (not vz) {
            CFGERROR("incorrect coordinate variable for dimention Z!");
            return false;
        }
    }

    return true;
}

void* StructuredMesh::GetCoordBuffer(int source , int iteration , int block ,
									 std::shared_ptr<Variable> var)
{
	std::shared_ptr<Block> b = GetCoordBlock(source , iteration , block , var);
	return b->GetDataSpace().GetData();
}

std::shared_ptr<Block> StructuredMesh::GetCoordBlock(int source , int iteration , int block ,
													 std::shared_ptr<Variable> var)
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
vtkDataArray* StructuredMesh::CreateTypedCoordArray(size_t length , T* pointer)
{
	vtkAOSDataArrayTemplate<T>* array = vtkAOSDataArrayTemplate<T>::New();
	array->SetNumberOfTuples(length);
	array->SetArray((T*)pointer , length , 1);

	return array;
}

vtkDataArray* StructuredMesh::CreateCoordArray(int source , int iteration , int block ,
                                               std::shared_ptr<Variable> var )
{
     model::Type type = var->GetLayout()->GetType();
	 size_t layoutSize = var->GetLayout()->GetNumberOfItems();

	 std::shared_ptr<Block>  b = GetCoordBlock(source ,  iteration , block , var);
	 void* pointer = b->GetDataSpace().GetData();

     switch(type) {
     case model::Type::short_:
		 return CreateTypedCoordArray<short>(layoutSize , (short*)pointer);
     case model::Type::int_:
     case model::Type::integer:
		 return CreateTypedCoordArray<int>(layoutSize , (int*)pointer);
     case model::Type::long_:
		 return CreateTypedCoordArray<long>(layoutSize , (long*)pointer);
     case model::Type::float_:
     case model::Type::real:
		 return CreateTypedCoordArray<float>(layoutSize , (float*)pointer);
     case model::Type::double_:
		 return CreateTypedCoordArray<double>(layoutSize , (double*)pointer);
     default:
         ERROR("Type is undefined for variable " << var->GetName());
     }

     return nullptr;
}

vtkDataSet* StructuredMesh::GetVtkGrid(std::shared_ptr<Variable> var , int source , int iteration , int block)
{
	std::shared_ptr<Variable> vx = nullptr;
	std::shared_ptr<Variable> vy = nullptr;
	std::shared_ptr<Variable> vz = nullptr;

	// Craeting the proper vtkGrid Object
	vtkDataSet* grid = CreateVtkGrid();

	// Getting the grid info
	if (not GetGridVariables(vx , vy , vz)) {
		ERROR("Failed to get variables related to the vtkGrid of: " << GetName());
		return nullptr;
	}

	// Setting the grid extents
	if (not SetGridExtents(grid , var , source ,  iteration ,  block)) {
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
