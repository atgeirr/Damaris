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

#ifndef UNSTRUCTUREDMESH_HPP
#define UNSTRUCTUREDMESH_HPP

#include "damaris/data/Mesh.hpp"

#ifdef HAVE_PARAVIEW_ENABLED
#include "damaris/paraview/ParaViewHeaders.hpp"
#endif

#include <vector>

namespace damaris {

class UnstructuredMesh :  public Mesh {

	friend class Deleter<UnstructuredMesh>;

private:
	/**
	 * Constructor, takes a model and a name as parameter.
	 * The constructor is private, the "New" static function should
	 * be used to create Rectilinear Meshes.
	 *
	 * \param[in] mdl : model from which to initialize the RectilinearMesh.
	 */
	UnstructuredMesh(const model::Mesh& mdl)
    : Mesh(mdl) {}

	/**
	 * Destructor.

	virtual ~UnstructuredMesh() override {
		if (this->vtk_usm_grid_ != nullptr )
			delete(this->vtk_usm_grid_) ;
	} */
	virtual ~UnstructuredMesh() {} ;

	/**
	 * The total number of (x,y,z) tuples to describe the vertex positions
	 * in 3d space
	 */
	size_t n_verticies_ ;
	// vtkNew<vtkUnstructuredGrid> vtkUSGrid_ ;
	// vtkUnstructuredGrid* vtk_usm_grid_ ;
	std::vector<vtkUnstructuredGrid* > vect_vtk_usm_grid_ ;

	/**
	 * used as an offset to select vtkUnstructuredGrid from vect_vtk_usm_grid_ container
	 */
	int source_range_low_;
	/**
	 * used to compute source_total_
	 */
	int source_range_high_;
	/**
	 * used to set the size of std::vector<vtkUnstructuredGrid* > vect_vtk_usm_grid_
	 */
	int source_total_ ;
public:
	     /**
		 * Creates an instance of UnstructuredMesh given a model and a name.
		 * Do some additional checking before creating it.
		 * If these verifications fail, return NULL.
		 *
		 * \param[in] mdl : base model from which to
		 */
		template<typename SUPER>
		static std::shared_ptr<SUPER> New(const model::Mesh& mdl,
					     const std::string &name)
		{

			if(mdl.name().find("/") != std::string::npos) {
				CFGERROR("Mesh " << mdl.name() << " cannot have a '/' character.");
				return std::shared_ptr<SUPER>();
			}

			if(mdl.type() != model::MeshType::unstructured) {
				return std::shared_ptr<UnstructuredMesh>();
			}
			std::shared_ptr<UnstructuredMesh> m = std::shared_ptr<UnstructuredMesh>(
					new UnstructuredMesh(mdl),
					Deleter<UnstructuredMesh>());
			m->name_ = name;
			m->n_verticies_ = 0 ;
			// m->vtk_usm_grid_ = nullptr ;
			m->vect_vtk_usm_grid_.resize(0);
			return m;
		}

		/**
		* Sets the range of client processes that are populating
		* the vector of vtkUnstructuredGrid* and resizes the vector to that size.
		* Only resizes vect_vtk_usm_grid_ on the first call.
		*
		* \param[in] source_range_low : lowest in the range of source (i.e. client rank) that the local server looks after
		* \param[in] source_range_high :  highest in the range of source (i.e. client rank) that the local server looks after
        *  N.B. The values for input are returned by Variable::GetSourceRange().
		*/
		virtual void SetSourceRange(int source_range_low, int source_range_high) override {

			if (vect_vtk_usm_grid_.size() == 0) {
				source_range_low_ = source_range_low ;
				source_range_high_ = source_range_high;
				source_total_ = source_range_high_ - source_range_low_ + 1 ;
				vect_vtk_usm_grid_.resize(source_total_, nullptr);  // explictly set to nullptr
				//for (vtkUnstructuredGrid* elem : vect_vtk_usm_grid_) elem = nullptr ;
			}
		}

#ifdef HAVE_PARAVIEW_ENABLED

	    /**
		* creates and returns the expected VTK grid object (vtkUnstructured) for a block
	    *
		* \param[in] source : source of the block
		* \param[in] iteration : iteration of the block
		* \param[in] block : id of the block
		* \param[in] var : the variable owning the block
	    */
		virtual vtkDataSet* GetVtkGrid(int source , int iteration , int block ,
									   const std::shared_ptr<Variable>& var) final;

		/**
		 * Sets the number of vertex coordinates that are part of the unstructured mesh
		 *
		 * \param[in] n_verticies
		 */
		void SetNVerticies(size_t n_verticies);


protected:
		/**
		 * Simply creates a vtkUnstructuredGrid and returns it.
		 */
		virtual vtkDataSet* CreateVtkGrid() override
		{

			vtkUnstructuredGrid*  vtk_usm_grid_ = vtkUnstructuredGrid::New();
			return vtk_usm_grid_;
			//vtkNew<vtkUnstructuredGrid> vtkUSGrid ;
			//return vtkUSGrid.GetPointer() ;
		}

		 vtkDataSet* CreateVtkGrid(int source)
		{

			vect_vtk_usm_grid_[source-source_range_low_] = vtkUnstructuredGrid::New();
			return vect_vtk_usm_grid_[source-source_range_low_] ;
			//vtkNew<vtkUnstructuredGrid> vtkUSGrid ;
			//return vtkUSGrid.GetPointer() ;
		}

		bool IsNull(int source)
		{
			if ( vect_vtk_usm_grid_[source-source_range_low_] == nullptr )
				return true;
			else
				return false ;
			//vtkNew<vtkUnstructuredGrid> vtkUSGrid ;
			//return vtkUSGrid.GetPointer() ;
		}

		vtkUnstructuredGrid* ReturnVTKMeshPtr(int source)
		{
			if (source-source_range_low_ <= (int) vect_vtk_usm_grid_.size())
				return vect_vtk_usm_grid_[source-source_range_low_] ;
			else
				return nullptr ;
		}


		std::shared_ptr<Block> ReturnBlock(int source , int iteration , int block ,
																 const std::shared_ptr<Variable>& var )
		{
			std::shared_ptr<Block> b = var->GetBlock(source , iteration , block);

			if (b == nullptr)  // no coord for this iteration
				b = var->GetBlock(source , 0 , block);

			if (b == nullptr) {
				ERROR("No block for variable " << var->GetName() << " in iteration " << iteration << std::endl );
				return nullptr;
			}

			return b ;
		}

		template<typename T>
		T * ReturnBlockDataPtr(int source , int iteration , int block ,
				 	 	 	 	 	 	 	 	 	 	 	 	 const std::shared_ptr<Variable>& var )
		{
			std::shared_ptr<Block> b = ReturnBlock( source, iteration,  block , var );
			if ( b == nullptr ) {
			    ERROR("The vertex data for variable " << var->GetName() << " in iteration " << iteration
				  << " does not an allocated Block " << std::endl );
				return nullptr ;
		   }
		   return (T *) b->GetDataSpace().GetData();
		}

		template<typename T>
		T * ReturnBlockDataPtr(std::shared_ptr<Block>& b )
		{
			if ( b == nullptr ) {
				ERROR("The Block provided is not allocated" << std::endl );
				return nullptr ;
		   }
		   return (T *) b->GetDataSpace().GetData();
		}
		/**
		 * This returns the three variable coordinates of the grid. Due to the
		 * dimension of the grid, some variables may be nullptr.
		 *
		 * \param[in,out] coords_xyz     : the coordinate variable for a vertices
		 *                                 as (x,y,z) coordinate tuples
		 * \param[in,out] vert_gid       : the global id's of the vertices
		 * \param[in,out] sect_vtk_type  : vector of mesh element types (VTK defined)
		 *                                 one for each section of the mesh
		 * \param[in,out] sect_sizes     : vector of mesh connectivity sizes in units
		 *                                 of the data type of the
		 *                                 one for each section of the complete mesh
		 * \param[in,out] sect_connect   : vector of mesh connectivities for each section
		 *                                 of the mesh
		 */
		bool GetGridVariables(	std::shared_ptr<Variable>& coords_xyz,
								std::shared_ptr<Variable>& vert_gid,
								std::shared_ptr<Variable>& sect_vtk_type,
								std::shared_ptr<Variable>& sect_vtk_sizes,
								std::shared_ptr<Variable>& sect_vert_connect ) ;

		/**
		 * Returns a  vtkPoints *  points array the based on the specified mesh verticies variable.
		 *
		 * \param[in] source : the source of the expected block
		 * \param[in] iteration : the Damaris iteration
		 * \param[in] block : the block id of the expected block
		 * \param[in] var : the variable (vert_gid). XML <mesh> element: <coord name="" unit="" label="" />
		 */
		vtkPoints * SetVertexCoords(int source , int iteration , int block ,
							 const std::shared_ptr<Variable>& varVerticies);


		/**
		 * Returns a  vtkIdTypeArray *  global ID array, based on specified mesh vert_gid variable
		 *
		 * \param[in] source : the source of the expected block
		 * \param[in] iteration : the Damaris iteration
		 * \param[in] block : the block id of the expected block
		 * \param[in] varGID : the variable (vert_gid). XML <mesh> element: <vertex_global_id name=""/>
		 */
		vtkIdTypeArray * SetGlobalIDs(int source , int iteration , int block ,
											 const std::shared_ptr<Variable>& varGID);

		/**
		 * Sets the connectivities for the vtkDataSet unstructured mesh, and (possibly) consists of
		 * multiple sections of (possibly) different mesh element types.
		 *
		 * \param[in,out] grid    : the VTK Unstructured mesh data set.
		 * \param[in] source      : the source of the expected block
		 * \param[in] iteration   : the Damaris iteration
		 * \param[in] block       : the block id of the expected block
		 * \param[in] section_vtk_type    : the array of VTK mesh element types (VTK_LINE, VTK_QUAD etc.),
		 *                                  one for each mesh section.
		 *                                  XML <mesh> element: <section_types name=""/>
		 * \param[in] section_vtk_sizes   : the variable array of sizes for the mesh sections. It is the number
		 *                                  of elements of the VTK type in this section. The mesh connectivity will then have
		 *                                  vtk_sizes_ptr[x] * strideof(vtk_type_ptr[x]) indices into the vertex data.
		 *                                  XML <mesh> element: <section_sizes name=""/>
		 * \param[in] vertex_connectivity : The array of indices of connectivity for each section of the mesh.
		 *                                  XML <mesh> element: <connectivity name=""/>
		 */
		bool SetVtkConnections(vtkDataSet* grid,  int source , int iteration , int block ,
				 const std::shared_ptr<Variable>& section_vtk_type,
				 const std::shared_ptr<Variable>& section_vtk_sizes,
				 const std::shared_ptr<Variable>& vertex_connectivity
				 );

		/**
		 * Returns the number of elements in a VTK_TYPE (i.e. VTK_QUAD returns 4)
		 * Returns -1 for VTK_POLYGON and VTK_POLYHEDRON
		 * and returns -2 if VTK_TYPE was not recognized.
		 *
		 * \param[in] VTK_TYPE : the VTK type (as defined in VTK vtkCellType.h)
		 */
		int strideofVtkType(int VTK_TYPE);


#endif // HAVE_PARAVIEW_ENABLED
}; // of class UnstructuredMesh

} // of namespace damaris

#endif // UNSTRUCTUREDMESH_HPP
