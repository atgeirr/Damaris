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

#ifndef __DAMARIS_MESH_H
#define __DAMARIS_MESH_H

#include "Damaris.h"

#ifdef HAVE_VISIT_ENABLED
	#include <VisItDataInterface_V2.h>
#endif

#ifdef HAVE_PARAVIEW_ENABLED
#include "damaris/paraview/ParaViewHeaders.hpp"
#endif



#include "damaris/util/Pointers.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Configurable.hpp"

#include "damaris/model/Model.hpp"

namespace damaris {


/**
 * The Mesh object is used for describing a mesh within a metadata structure.
 */
class Mesh : public Configurable<model::Mesh> {

	friend class Deleter<Mesh>;
	friend class Manager<Mesh>;
	/**
	 *  Looks for a variable by name (aka std::string) within
	 *  the VariableManager but adds each group prefix to the name
	 *
	 *  Note: Used by FindVar if the variable being searched for is not
	 *  a fully qualified group/name combination.
	 *
	 * \param[in] varName : The variable name (without a group prefix)
	 */
	std::shared_ptr<Variable> FindVarInGroup(std::string& varName) ;

	protected:
		int id_; /*! id of the mesh, provided by the MeshManager. */
		std::string name_; /*! Name of the mesh, copy from the model. */
		std::vector<std::shared_ptr<Variable> > coords_; /*! pointer to coordinate variables. */

	protected:
		
		/**
		 * Constructor, is protected (cannot be instantiated by outside,
		 * allows child classes).
		 * 
		 * \param[in] mdl : model from which to initialize the Mesh.
		 */
		Mesh(const model::Mesh& mdl)
		: Configurable<model::Mesh>(mdl), id_(-1), name_(mdl.name()) {}

		/**
		 * Destructor.
		 */
		virtual ~Mesh() {}

		/**
		 * Looks for a variable by name (aka std::string) within
		 * the VariableManager. Used to return the variables related to a mesh
		 *
		 * \param[in] varName : visit handle to fill.
		 */
		std::shared_ptr<Variable> FindVar(std::string& varName);


	public:
		/**
		 * Returns the name of the Mesh.
		 */
		const std::string& GetName() const {
			return name_;
		}

		/**
		 * Returns the id of the Mesh.
		 */
		int GetID() const { return id_; }

		/**
		 * Returns the nth coordinate variable.
		 * Specified by the (sequence) XML tag: <coord  name="" unit="" label="" />
		 * Will search for the coordinates only once and store the
		 * the result for later calls.
		 * If there is only a single coordinate variable, it is assumed to be for an
		 * Unstructured mesh and contains (x,y,z) vertex position tuples. Otherwise
		 * there should be 2 or 3 coordinate arrays, one for x, one for  y and an
		 * optional one for z vertex values. These are used for Rectilinear grid
		 * definitions.
		 *
		 * \param[in] n : index of the coordinate.
		 */
		std::shared_ptr<Variable> GetCoord(unsigned int n);

		/**
		 * Returns the vertex global ID variable (if it exists) which is specified by the
		 * XML tag <vertex_global_id   name="" offset=""  />
		 * This is the vector of global ID's of each vertex that is found in the coordinate
		 * variable. These ID's are used in the VertexConnectivity vector to specify what
		 * vertex is connected to which other.
		 */
		 std::shared_ptr<Variable> GetVertexGID();

		 /**
		 * Returns the vector of VTK types (if it exists) of each section
		 * of an Unstructured grid.
		 * Specified by the XML tag: <section_types  name=""  />
		 */
		 std::shared_ptr<Variable> GetSectionVTKType();

		 /**
		 * Returns the vector of sizes (if it exists) of each section
		 * of an Unstructured grid.
		 * Specified by the XML tag:   <section_sizes  name=""  />
		 * The size is defined as the number of elements of the VTK
		 * type mulitplied by the 'stride' of the element. The stride is the number of values
		 * required to specify a single complete VTK structural type (e.g. VTK_LINE = 2,
		 * VTK_QUAD = 4, VTK_HEXAHEDRON = 8, etc.)
		 */
		 std::shared_ptr<Variable> GetSectionSizes();

		 /**
		 * Returns the variable that contains the vertex connectivities of an Unstructured mesh.
		 * Specified by the XML tag: <connectivity   name=""  />
		 * The variable may consist of multiple sections (the number of sections being the length
		 * of the section_types variable (return value of GetSectionVTKType()) and the size of
		 * each section being described by entries in the section_sizes variable (the return
		 * value of GetSectionSizes()).
		 */
		 std::shared_ptr<Variable> GetSectionVertexConnectivity();


		/**
		 * Returns the number of coordinates.
		 */
		unsigned int GetNumCoord() const {
			return GetModel().coord().size();
		}

		/**
		 * Returns the topological dimension.
		 */
		unsigned int GetTopology() const {
			return GetModel().topology();
		}


#ifdef HAVE_VISIT_ENABLED
        /**
		 * Fills visit handle with metadata related to the Mesh.
		 *
		 * \param[in] md : visit handle to fill.
		 */
		virtual bool ExposeVisItMetaData(visit_handle md) = 0;
		
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 * 
		 * \param[in] h : visit handle to fill.
		 * \param[in] source : source to expose.
		 * \param[in] iteration : iteration to expose.
		 * \param[in] block : domain to expose.
		 */
		virtual bool ExposeVisItData(visit_handle* h, int source, 
				int iteration, int block) = 0;
#endif

#ifdef HAVE_PARAVIEW_ENABLED



		/**
		* creates and returns the expected VTK grid object for a block
		*
		* \param[in] source : source of the block
		* \param[in] iteration : iteration of the block
		* \param[in] block : id of the block
		* \param[in] var : the variable owning the block
		*/
		virtual vtkDataSet* GetVtkGrid(int source , int iteration , int block ,
									   const std::shared_ptr<Variable>& var) = 0;

	protected:
	    /**
	     * Every derived class, should implement this method and
	     * create the appropriate vtkDataSet and return it.
	     */
	    virtual vtkDataSet* CreateVtkGrid() = 0;

#endif
};

}
#endif
