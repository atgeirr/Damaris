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
	 * The number of section in each block that is looked after
	 * by the current server core
	 */
	std::vector<int > vect_server_sctns_numberof_ ;

	/**
	 * The number of section in each block that is looked after
	 * by the current server core
	 */
	std::vector<int > vect_server_sctn_offsets_ ;

	/**
	 *  The source that was called in the last call to the function IsNull()
	 */
	int last_source_;
	/**
	 *  If the source is the same as last_source_ then Increments, otherwise reset to 0
	 */
	int times_called_for_current_source_;

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

	std::shared_ptr<Variable> sect_vtk_sizes_ ;
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
			m->last_source_ = -1 ;
			m->times_called_for_current_source_ = 0 ;
			return m;
		}

		/**
		* Sets the range of client processes that are populating
		* the vector of vtkUnstructuredGrid* and resizes the vector to that size.
		* Only resizes vect_vtk_usm_grid_ on the first call.
		*
		* \param[in] source_range_low : lowest in the range of source (i.e. client communicator rank) that the local server looks after
		* \param[in] source_range_high :  highest in the range of source (i.e. client communicator rank) that the local server looks after
        *  N.B. The values for input are returned by Variable::GetSourceRange().
		*/
		virtual void SetSourceRange(int source_range_low, int source_range_high) override {

			if (vect_vtk_usm_grid_.size() == 0) {
				source_range_low_  = source_range_low ;
				source_range_high_ = source_range_high;
				source_total_      = source_range_high_ - source_range_low_ + 1 ;

				// get the variable of the mesh that holds the section sizes
				sect_vtk_sizes_    = GetSectionSizes();
				int         num_sections ;
				int         section_size ;
				int         num_sections_total = 0;  // the number of vtkMultiPieceDataSet sections required for this server
				const int   iteration = 0 ;  // currently the mesh is unchanging so we only need data from iteration 0. This is a big assumption!
				const int   block = 0 ;
				// Loop over (the first) block from each source that is being looked after by this server.
				// We assume there is only one block ber source for the UnstructuredMesh data Variable sect_vtk_sizes.
				for(int source = source_range_low_; source <= source_range_high_; source++) {
					std::shared_ptr<Block> sectn_block;
					vect_server_sctn_offsets_.push_back(num_sections_total);
					// Get the block (that corresponds to the block of the enclosing field Variable)
					sectn_block =  sect_vtk_sizes_->GetBlock(source , iteration , block) ;
					// Get the number of sections of the mesh held by this block
					// - there is 1 array element per mesh section.
					num_sections = sectn_block->GetNumberOfItems();
					num_sections_total += num_sections ;
					vect_server_sctns_numberof_.push_back(num_sections);
					INFO("SetSourceRange mesh     Mesh:" << GetName() <<  " source: " << source  << "  num_sections: " << num_sections <<  "  num_sections_total: " << num_sections_total );
				}

				vect_vtk_usm_grid_.resize(num_sections_total, nullptr);  // explictly set to nullptr

			}
		}

		int GetNumberOfSections(int source)  {
			int offset_to_source =  LocalServerSourceIndex(source) ;
			return vect_server_sctns_numberof_[offset_to_source] ;
		}

		int GetSectionOffset(int source) {
			int offset_to_source =  LocalServerSourceIndex(source) ;
			return vect_server_sctn_offsets_[offset_to_source] ;
		}

		int GetSectionSize(int source, int sectn_num) {
			// int offset_to_source =  LocalServerSourceIndex(source) ;
			// Could check that sectn_num is a valaid index?
			int iteration = 0 ;
			int block = 0 ;
			int * sectn_size_ptr ;

			//std::shared_ptr<Variable> sect_vtk_sizes ;  // we use this variable a lot, so maybe retrieve and cache?
			std::shared_ptr<Block> sectn_block;
			//sect_vtk_sizes  = GetSectionSizes();  // returns a Variable
			// Get the block (that corresponds to the block of the enclosing field Variable)
			sectn_block =  sect_vtk_sizes_->GetBlock(source , iteration , block) ;
			// Get a pointer to the section block
			sectn_size_ptr = (int *) sectn_block->GetDataSpace().GetData();

			return sectn_size_ptr[sectn_num] ;


		}

		void ResetSectionOffset( void )
		{
			times_called_for_current_source_ = 0 ;
		}

#ifdef HAVE_PARAVIEW_ENABLED

	    /**
		* creates and returns the expected VTK grid object (vtkUnstructured) for a block
	    *
		* \param[in] source : source of the block  (mpi rank within the client communicator)
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


		/**
		 * Returns an offset into  std::vector<int > vect_server_sctn_offsets_ that corresponds to
		 * the source ( source is an mpi rank within the client communicator )
		 *
		 * \param[in] source : The source of the block  (i.e. mpi rank within the client communicator)
		 */
		int LocalServerSourceIndex(int source) {
			//assert(source-source_range_low_ >= 0)
			return source-source_range_low_;
		}

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

		/**
		 * Creates a vtkUnstructuredGrid places it in the correct place of the std::vector<> taking in to account
		 * the source and the number of section within the block of the source and then returns it.
		 */
		 vtkDataSet* CreateVtkGrid(int source)
		{
			int offset_to_source = LocalServerSourceIndex(source)  ;
			int offset = vect_server_sctn_offsets_[offset_to_source] + times_called_for_current_source_++  ;

			vect_vtk_usm_grid_[offset] = vtkUnstructuredGrid::New();
			return vect_vtk_usm_grid_[offset] ;
			//vtkNew<vtkUnstructuredGrid> vtkUSGrid ;
			//return vtkUSGrid.GetPointer() ;
		}

		 /**
		 *  has to be called each time a vtkUnstructuredGrid is required. It sets the
		 *  correct offset for the section. Not threadsafe and should not be called
		 *  in more than one function
		 */
		bool IsNull(int source)
		{
			int offset_to_source = LocalServerSourceIndex(source) ; // get local server source index
			int sects_in_source  = vect_server_sctns_numberof_[offset_to_source];


			if ((times_called_for_current_source_ >= sects_in_source) || (source != last_source_)) {
			   ResetSectionOffset() ; // resets times_called_for_current_source_ = 0 ;
			   last_source_ = source ;
			}

			int offset = vect_server_sctn_offsets_[offset_to_source]  + times_called_for_current_source_ ;


			if ( vect_vtk_usm_grid_[offset] == nullptr )
				return true;
			else
				return false ;
			//vtkNew<vtkUnstructuredGrid> vtkUSGrid ;
			//return vtkUSGrid.GetPointer() ;
		}

		vtkUnstructuredGrid* ReturnVTKMeshPtr(int source)
		{
			int offset_to_source = LocalServerSourceIndex(source)  ;
			int offset = vect_server_sctn_offsets_[offset_to_source]  + times_called_for_current_source_++ ;

			if (offset <= (int) vect_vtk_usm_grid_.size())
				return vect_vtk_usm_grid_[offset] ;
			else
				return nullptr ;
		}

		/**
		 *  Return the total number of sections in the mesh - this is done by looking at the
		 *  number of elements in the 'groupname/section_sizes' Variable, so requires a Block
		 *  of the Variable. Each server will have to call this and use there own version of the
		 *  Variable.
		 *  Need to use the block based info. as it has the global size updated when
		 *  damaris_paramater_set(n_sections_total) is called to modify the layout.
		 *
		 */
		int GetTotalMeshSections(int iteration) {
			std::shared_ptr<Variable> sect_sizes_var;
			std::shared_ptr<Block> sectn_block;
			int source_range_low, source_range_high ;
			int    n_sections_total  = 1;

			// get the variable of the mesh that holds the section sizes
			sect_sizes_var = GetSectionSizes() ;
			sect_sizes_var->GetSourceRange(source_range_low, source_range_high);
			int block = 0 ;     // always a single block for mesh variables.
			int dimension = 0 ; // always a single dimension variable

			// Get the block (that corresponds to the block of the enclosing field Variable)
			sectn_block =  sect_sizes_var->GetBlock(source_range_low , 0 , block) ;
			// Get the number of sections of the mesh held by this block
			// - there is 1 array element per mesh section.
			n_sections_total = sectn_block->GetGlobalExtent(0);
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
