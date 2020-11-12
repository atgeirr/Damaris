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
	 */
	virtual ~UnstructuredMesh() {}

	/**
	 * The total number of (x,y,z) tuples to describe the vertex positions
	 * in 3d space
	 */
	size_t n_verticies_ ;


public:
	     /**
		 * Creates an instance of RectilinearMesh given a model and a name.
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
			return m;
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
		 * Simply creates a vtkUnstructuredGrid and return it.
		 */
		virtual vtkDataSet* CreateVtkGrid() override
		{
			return vtkUnstructuredGrid::New();
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


#endif // HAVE_PARAVIEW_ENABLED
}; // of class UnstructuredMesh

} // of namespace damaris

#endif // UNSTRUCTUREDMESH_HPP
