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
#ifndef __DAMARIS_POINT_MESH_H
#define __DAMARIS_POINT_MESH_H

#include "Damaris.h"
#include "damaris/util/Deleter.hpp"
#include "damaris/data/Mesh.hpp"

namespace damaris {


/**
 * PointMesh is a particular type of Mesh that handles a cloud of non-linked
 * points. The coordinates of the points are handled by 2 or 3 arrays, and
 * any 1D variablable can be mapped to this type of mesh.
 */
class PointMesh : public Mesh {
	
	friend class Deleter<PointMesh>;

private:
	/**
	 * Constructor, takes a model and a name as parameter. The constructor 
	 * is private, the "New" static function should be used to create 
	 * Rectilinear Meshes.
	 *
	 * \param[in] mdl : model from which to initialize the PointMesh.
	 */
	PointMesh(const model::Mesh& mdl) : Mesh(mdl) {}
	
	/**
	 * Destructor.
	 */
	virtual ~PointMesh() {}
	
public:
	
	/**
	 * Creates an instance of PointMesh given a model. 
	 * Do some additional checking before creating it. If these 
	 * verifications fail, return NULL.
	 *
	 * \param[in] mdl : model from which to create the PointMesh.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Mesh& mdl,
					const std::string& name)
	{

		if(mdl.name().find("/") != std::string::npos) { 
			CFGERROR("Mesh " << mdl.name() << " cannot have a '/' character.");
			return std::shared_ptr<SUPER>();
		}

		if(mdl.type() == model::MeshType::point) {
			std::shared_ptr<PointMesh> m = 
				std::shared_ptr<PointMesh>(new PointMesh(mdl),
				Deleter<PointMesh>());
			m->name_ = name;
			return m;
		} else {
			return std::shared_ptr<SUPER>();
		}
	}


#ifdef HAVE_VISIT_ENABLED
	/**
	 * Fills the visit handle with Metadata related to this Mesh.
	 * Returns true in case of success.
	 *
	 * \param[in] md : visit handle to fill.
	 */
	bool ExposeVisItMetaData(visit_handle md);
		
	/**	
	 * Fills the visit handle with the data related to this Mesh.
	 * Returns true is successful. This is a rare function that
	 * still takes a pointer to something instead of a reference
	 * or a smart pointer because of the way VisIt works.
	 *
	 * \param[out] h : visit handle to fill.
	 * \param[in] source : source to expose to visit.
	 * \param[in] iteration : iteration to expose to visit.
	 * \param[in] block : domain to expose to visit.
	 */
	bool ExposeVisItData(visit_handle* h, 
		int source, int iteration, int block);
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
									   const std::shared_ptr<damaris::Variable>& var) override;
#endif

};

}
#endif
