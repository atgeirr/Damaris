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

	// Create the proper vtkGrid Object, dependent on grid type (in this case, unstructured)
	// Uses virtual method dispatch of Mesh::GetVtkGrid() class
	vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast
	        (vtkDataSet::SafeDownCast(CreateVtkGrid()));

	// Getting the grid info
	if (not GetGridVariables(coords_xyz ,
							 vertex_gid ,
							 section_vtk_type,
							 section_vtk_sizes,
							 vertex_connectivity)) {
		ERROR("Failed to get variables related to the vtkGrid of: " << GetName());
		return nullptr;
	}

	// Setting the vertex point coordinates
	vtkPoints * vtkPoints = SetVertexCoords( source ,  iteration ,  block , coords_xyz) ;
	if (vtkPoints == nullptr ) {
		ERROR("Setting the unstructured mesh vertex coordinates failed: Mesh name: " << GetName()) ;
		return nullptr;
	}

	//Setting the global ID's
	vtkIdTypeArray * vtkGid =  SetGlobalIDs(  source , iteration , block ,vertex_gid) ;
	if (vtkGid == nullptr ) {
		ERROR("Setting the unstructured mesh global ids failed! Mesh name: " << GetName()) ;
		return nullptr;
	}

	grid->GetPointData()->SetGlobalIds(vtkGid) ;
	vtkGid->Delete() ;

	grid->SetPoints(vtkPoints) ;
	vtkPoints->Delete() ;

	// This does a loop over all sections, adding the connectivities which are specific for
	// each vtk mesh element type
	if (not SetVtkConnections(grid, source, iteration, block,
			                  section_vtk_type,
							  section_vtk_sizes,
							  vertex_connectivity)) {
			ERROR("Failed toset the vertex connections in the vtkGrid of: " << GetName());
			return nullptr;
		}

	return grid;
}


bool UnstructuredMesh::SetVtkConnections(vtkDataSet* grid,  int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& section_vtk_type,
									 const std::shared_ptr<Variable>& section_vtk_sizes,
									 const std::shared_ptr<Variable>& vertex_connectivity
									 )
{
	size_t vtk_num_sections ; // This is the size of the section_vtk_type and section_vtk_sizes Variables
	const int * vtk_type_ptr ;
	const int * vtk_sizes_ptr ;
	const unsigned long * vrtx_connect_ptr ;

    vtkUnstructuredGrid* uGrid = vtkUnstructuredGrid::SafeDownCast(grid);

    if (uGrid == nullptr) {
    		ERROR("Cannot downcast the parameter grid to vtkUnstructuredGrid");
    		return false;
    }

    // Get the array of VTK mesh element types (VTK_LINE, VTK_QUAD etc.), one element type for each mesh section
    if (section_vtk_type->GetLayout()->GetType() == model::Type::int_) {
		std::shared_ptr<Block> b = ReturnBlock(source ,  iteration ,  block , section_vtk_type) ;
		vtk_type_ptr             = static_cast<const int *>(ReturnBlockDataPtr<int>( b )) ;
		vtk_num_sections         = b->GetNumberOfItems();
    } else {
		ERROR("The section type for mesh " << GetName()  << " variable named: " << section_vtk_type->GetName()
				<< " in iteration " << iteration << " does not have the correct data type (requires int) " << std::endl );
		return false ;
	}

    // Get the array of sizes for the mesh sections. It is the number of elements of the VTK type in this section
    // The mesh connectivity will then have vtk_sizes_ptr[x] * strideof(vtk_type_ptr[x]) indices into the vertex data.
    if (section_vtk_sizes->GetLayout()->GetType() == model::Type::int_) {
		std::shared_ptr<Block> b = ReturnBlock(source ,  iteration ,  block , section_vtk_sizes) ;
		vtk_sizes_ptr            = static_cast<const int *>(ReturnBlockDataPtr<int>( b )) ;
		if (vtk_num_sections != b->GetNumberOfItems()) { // check they are the same size (they should use the same layout)
			ERROR("The section size for mesh " << GetName()  << " variable named: " << section_vtk_sizes->GetName()
					<< " in iteration " << iteration << " does not match the section size or the vtk_type variable: "
					<< section_vtk_type->GetName() << std::endl );
					return false ;
		}
    } else {
		ERROR("The section type for mesh " << GetName() << " variable named:  " << section_vtk_sizes->GetName()
				<< " in iteration " << iteration << " does not have the correct data type (requires int) " << std::endl );
		return false ;
	}

    long int n_elts = 0 ;
    for (size_t t1 = 0 ; t1 < vtk_num_sections; t1++)
    {
    	n_elts += vtk_sizes_ptr[t1] ;
    }
	if (n_elts > 0)
		uGrid->Allocate(n_elts);


    // Get the array of mesh connectivities.
	if (vertex_connectivity->GetLayout()->GetType() == model::Type::long_) {
		std::shared_ptr<Block> b = ReturnBlock(source ,  iteration ,  block , section_vtk_sizes) ;
		vrtx_connect_ptr         = static_cast<const unsigned long *>(ReturnBlockDataPtr<const unsigned long>( b )) ;
	} else {
		ERROR("The section type for mesh " << GetName()  << " variable named: " << section_vtk_sizes->GetName()
				<< " in iteration " << iteration << " does not have the correct data type (requires long) " << std::endl );
		return false ;
	}

	int vtk_type_stride ;
	int vtk_type ;
	int vert_gid_offset = GetModel().vertex_global_id().get().offset() ; // can also request offset()
	vtkIdType *vtx_ids = new vtkIdType[8]; // 8 is the maximum size required

	size_t sectn_offset = 0 ;
	for (size_t sectn_id = 0; sectn_id < vtk_num_sections; sectn_id++) {
		vtk_type         = vtk_type_ptr[sectn_id] ;
		vtk_type_stride  = strideofVtkType(vtk_type);
	    if (vtk_type_stride > 0) {
	    	  for (int t1 = 0; t1 < vtk_sizes_ptr[sectn_id]; t1++) {
	    		  for (int t2 = 0; t2 < vtk_type_stride; t2++)
	    			  vtx_ids[t2] = vrtx_connect_ptr[sectn_offset + (t1*vtk_type_stride) + t2] + vert_gid_offset;
	    		  uGrid->InsertNextCell(vtk_type, vtk_type_stride, vtx_ids);
	    	  }
	    	  sectn_offset += vtk_sizes_ptr[sectn_id] * vtk_type_stride;
	    }
	    else if (vtk_type == VTK_POLYGON) {
	    	// _export_nodal_polygons(section, ugrid);
	    	ERROR("The section type for mesh " << GetName()  << " is VTK_POLYGON which is not implemented in current (development) version " << std::endl );
	    	return false ;
	    }
	    else if (vtk_type == VTK_POLYHEDRON) {
	    	// _export_nodal_polyhedra(mesh->n_vertices, section, ugrid);
	    	ERROR("The section type for mesh " << GetName()  << " is VTK_POLYHEDRON which is not implemented in current (development) version " << std::endl );
	    	return false ;
	    }


	  } /* End of loop on sections */

	delete [] vtx_ids;

	return true ;

}



vtkPoints * UnstructuredMesh::SetVertexCoords( int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& varVerticies)
{

	const double * vertexdata ;
	size_t vertexSize ;
	// Assuming model::Type::double_:
	//double * vertexdata = (double *) b->GetDataSpace().GetData();
	if (varVerticies->GetLayout()->GetType() == model::Type::double_) {
		std::shared_ptr<Block> b = ReturnBlock(source ,  iteration ,  block , varVerticies) ;
		vertexdata               = static_cast<const double *>(ReturnBlockDataPtr<double>( b ) );
		vertexSize                = b->GetNumberOfItems();
	} else {
		ERROR("The vertex data for variable " << varVerticies->GetName() << " in iteration " << iteration
						<< " does not have the correct data type (requires double) " << std::endl );
		return nullptr ;
	}

	// The size of the data block should be 3 x n_verticies_for topology 3 mesh
	// or 2 x n_verticies_for topology 2 mesh.
	// This is enforced through the [][2 | 3] layout

	int topology = GetTopology() ;

	if (topology == 3) {
		if ((vertexSize % 3) != 0) { // in case the layout is not correct
			ERROR("The unstructured coordinate blocks for variable " << varVerticies->GetName() << " in iteration " << iteration
				<< " does not have the correct number of coordinates (not a factor of 3 for x,y,z coordinates) "
				  << std::endl );
			return nullptr ;
		}

		n_verticies_ = vertexSize / 3 ;

	} else if (topology == 2) {
		if ((vertexSize % 2) != 0) { // in case the layout is not correct
			ERROR("The unstructured coordinate blocks for variable " << varVerticies->GetName() << " in iteration " << iteration
				<< " does not have the correct number of coordinates (not a factor of 2 for x,y coordinates) "
				  << std::endl );
			return nullptr ;
		}

		n_verticies_ = vertexSize / 2 ;
	} else {
		ERROR("The topology value for the mesh is not a valide value (2 or 3)" << std::endl );
		return nullptr ;
	}

    double point[3] ;
	vtkPoints * points = vtkPoints::New() ;
	points->Allocate(n_verticies_) ;

	if (topology == 2)
		point[2] = 0.0 ;

	for (size_t t1 = 0 ; t1 < n_verticies_ ; t1++){

		for (int t2 = 0 ; t2 < topology ; t2++)
			point[t2] = vertexdata[t1*topology + t2] ;

		points->InsertNextPoint(point[0],point[1],point[2]) ;
	}

	return points;
}

vtkIdTypeArray * UnstructuredMesh::SetGlobalIDs(int source , int iteration , int block ,
									 const std::shared_ptr<Variable>& varGID)
{

	vtkIdTypeArray * global_vtx_ids = vtkIdTypeArray::New() ;

	int vert_gid_offset = GetModel().vertex_global_id().get().offset() ; // the offset if needed

	global_vtx_ids->SetNumberOfComponents(1);
	global_vtx_ids->SetName("GlobalNodeIds");
	if (n_verticies_ == 0) {
		ERROR(" The n_verticies_ has not been set! Mesh is named: " << GetName() << ". (Has SetVertexCoords() been called first?) " << std::endl) ;
		return nullptr ;
	}
	global_vtx_ids->SetNumberOfTuples(n_verticies_);


	std::shared_ptr<Block> b = varGID->GetBlock(source , iteration , block);

	if (b == nullptr)  // no coord for this iteration
		b = varGID->GetBlock(source , 0 , block);

	if (b == nullptr) {
		ERROR("No coordinate blocks for variable " << varGID->GetName() << " in iteration " << iteration << std::endl );
		return nullptr;
	}

	// Assuming model::Type::long_:
	// std::shared_ptr<Layout> var_GID_layout = ;

	if (varGID->GetLayout()->GetType()  == model::Type::long_)
	{
		long int * giddata = (long int *) b->GetDataSpace().GetData();

		for (size_t t1 = 0 ; t1 < n_verticies_ ; t1++){
			vtkIdType idt = static_cast<unsigned long int>(giddata[t1]) + vert_gid_offset ;

			global_vtx_ids->SetTypedTuple(t1, &idt) ;
		}
	} else if (varGID->GetLayout()->GetType()  == model::Type::int_) {
		 int * giddata = ( int *) b->GetDataSpace().GetData();

		for (size_t t1 = 0 ; t1 < n_verticies_ ; t1++){
			vtkIdType idt = static_cast<unsigned int>(giddata[t1]) + vert_gid_offset ;

			global_vtx_ids->SetTypedTuple(t1, &idt) ;
		}
	}

	return global_vtx_ids ;
}


int UnstructuredMesh::strideofVtkType(int VTK_TYPE)
{
  // case values are defined in VTK library header vtkCellType.h
  switch(VTK_TYPE) {

  case VTK_LINE:
    return 2 ;
    break;

  case VTK_TRIANGLE:
	  return 3 ;
    break;

  case VTK_QUAD:
	  return 4 ;
    break;

  case VTK_TETRA:
	  return 4 ;
    break;

  case VTK_PYRAMID:
	  return 5 ;
    break;

  case VTK_WEDGE:
	  return 6 ;
    break;

  case VTK_HEXAHEDRON:
	  return 8 ;
    break;

  case VTK_POLYGON:
	  return -1 ;;
    break;

  case VTK_POLYHEDRON:
	return -1;
    break;

  default:
	  ERROR("Stride for VTK_TYPE: " << VTK_TYPE << " was not valid" << std::endl );
  }

  return -2;
}


#endif


} // of namespace damaris
