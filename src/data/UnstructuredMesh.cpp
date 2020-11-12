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

void UnstructuredMesh::SetNVerticies(size_t n_verticies)
{
	// The total number of (x,y,z) tuples to describe the vertex positions
	// in 3d space
	n_verticies_ = n_verticies ;
}

#ifdef HAVE_PARAVIEW_ENABLED

bool UnstructuredMesh::GetGridVariables(std::shared_ptr<Variable>& coords_xyz,
										std::shared_ptr<Variable>& vert_gid,
										std::shared_ptr<Variable>& sect_vtk_type,
										std::shared_ptr<Variable>& sect_vtk_sizes,
										std::shared_ptr<Variable>& sect_vert_connect )
{
    unsigned int numCoord = GetNumCoord();
    if (numCoord != 1) {
        CFGERROR("Type UnstructuredMesh should have a single coord, with [][3] "
        		"(for topology==3) or [][2] (for topology==2) layout dimension");
        return false;
    }

    coords_xyz        = GetCoord(0);
    vert_gid          = GetVertexGID();
    sect_vtk_type     = GetSectionVTKType();
    sect_vtk_sizes    = GetSectionSizes();
    sect_vert_connect = GetSectionVertexConnectivity();

    return true;
}



vtkDataSet* UnstructuredMesh::GetVtkGrid(int source , int iteration , int block ,
									   const std::shared_ptr<Variable>& var)
{
	std::shared_ptr<Variable> coords_xyz = nullptr;
	std::shared_ptr<Variable> vertex_gid = nullptr;
	std::shared_ptr<Variable> section_vtk_type = nullptr;
	std::shared_ptr<Variable> section_vtk_sizes = nullptr;
	std::shared_ptr<Variable> vertex_connectivity = nullptr;

	// Creat the proper vtkGrid Object, dependent on grid type (in this case, unstructured)
	// Uses virtual method dispatch of Mesh::GetVtkGrid() class
	vtkDataSet* grid = CreateVtkGrid();

	// Getting the grid info
	if (not GetGridVariables(coords_xyz , vertex_gid , section_vtk_type, section_vtk_sizes, vertex_connectivity)) {
		ERROR("Failed to get variables related to the vtkGrid of: " << GetName());
		return nullptr;
	}

	// Setting the vertex point coordinates
	vtkPoints * vtkPoints = SetVertexCoords( source ,  iteration ,  block , coords_xyz)
	if (vtkPoints == nullptr ) {
		ERROR("Setting the unstructured mesh vertex coordinates failed.");
		return nullptr;
	}

	//Setting the global ID's
	vtkIdTypeArray * vtkGid =  SetGlobalIDs(  source , iteration , block ,vertex_gid)
	if (vtkGid == nullptr ) {
		ERROR("Setting the unstructured mesh global ids failed!");
		return nullptr;
	}

	grid->GetPointData()SetGlobalIds(vtkGid) ;
	vtkGid->Delete() ;

	grid->SetPoints(vtkPoints) ;
	vtkPoints->Delete() ;



	return grid;
}


vtkPoints * UnstructuredMesh::SetVertexCoords( int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& varVerticies)
{

   // vtkUnstructuredGrid* uGrid = vtkUnstructuredGrid::SafeDownCast(grid);

	if (uGrid == nullptr) {
		ERROR("Cannot downcast the parameter grid to vtkUnstructuredGrid");
		return nullptr;
	}

	std::shared_ptr<Block> b = varVerticies->GetBlock(source , iteration , block);

	if (b == nullptr)  // no coord for this iteration
		b = varVerticies->GetBlock(source , 0 , block);

	if (b == nullptr) {
		ERROR("No coordinate blocks for variable " << varVerticies->GetName() << " in iteration " << iteration << std::endl );
		return nullptr;
	}

	// Assuming model::Type::double_:
	double * vertexdata = (double *) b->GetDataSpace().GetData();

	// The size of the data block should be 3 x n_verticies_for topology 3 mesh
	// or 2 x n_verticies_for topology 2 mesh.
	// This is enforced through the [][2 | 3] layout
	size_t blockSize = b->GetNumberOfItems();

	int topology = GetTopology() ;

	if (topology == 3) {
		if ((blockSize % 3) != 0) { // in case the layout is not correct
			ERROR("The unstructured coordinate blocks for variable " << varVerticies->GetName() << " in iteration " << iteration
				" does not have the correct number of coordinates (not a factor of 3 for x,y,z coordinates) "
				  << std::endl );
			return nullptr ;
		}

		n_verticies_ = blockSize / 3 ;

	} else if (topology == 2) {
		if ((blockSize % 2) != 0) { // in case the layout is not correct
			ERROR("The unstructured coordinate blocks for variable " << varVerticies->GetName() << " in iteration " << iteration
				" does not have the correct number of coordinates (not a factor of 2 for x,y coordinates) "
				  << std::endl );
			return nullptr ;
		}

		n_verticies_ = blockSize / 2 ;
	}

    double point[3] ;
	vtkPoints * points = vtkPoints::New() ;
	points->Allocate(n_verticies_) ;

	if (topology == 2)
		point[2] = 0.0 ;

	for (size_t t1 = 0 ; t1 < n_verticies_ ; t1++){

		for (t2 = 0 ; t2 < topology ; t2++)
			point[t2] = vertexdata[t1*topology + t2] ;

		points->InsertNextPoint(point[0],point[1],point[2]) ;
	}

	return points;
}

vtkIdTypeArray * UnstructuredMesh::SetGlobalIDs(int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& varGID)
{

	vtkIdTypeArray * global_vtx_ids = vtkIdTypeArray::New() ;

	int vert_gid_offset = GetModel().vertex_global_id().get().offset() ; // can also request offset()

	global_vtx_ids->SetNumberOfComponents(1);
	global_vtx_ids->SetName("GlobalNodeIds");
	if (n_verticies_ == 0) {
		ERROR(" The n_verticies_ has not been set! Mesh is named: " << GetName() ". (Has SetVertexCoords() been called first?) " << std::endl) ;
		return nullptr ;
	}
	global_vtx_ids->SetNumberOFTuples(n_verticies_);

	std::shared_ptr<Block> b = varGID->GetBlock(source , iteration , block);

	if (b == nullptr)  // no coord for this iteration
		b = varGID->GetBlock(source , 0 , block);

	if (b == nullptr) {
		ERROR("No coordinate blocks for variable " << var->GetName() << " in iteration " << iteration << std::endl );
		return false;
	}

	// Assuming model::Type::double_:
	double * giddata = (double *) b->GetDataSpace().GetData();


	for (size_t t1 = 0 ; t1 < n_verticies_ ; t1++){
		vtkIdType idt = giddata[i] + vert_gid_offset ;

		global_vtx_ids->setTypedTuple(t1, &idt) ;
	}

	return global_vtx_ids ;
}


#endif


} // of namespace damaris
