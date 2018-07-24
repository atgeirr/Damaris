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

#ifndef __DAMARIS_RECTILINEAR_MESH_H
#define __DAMARIS_RECTILINEAR_MESH_H

#include "Damaris.h"

#include "damaris/data/Mesh.hpp"

namespace damaris {


/**
 * RectilinearMesh is a particular type of Mesh.
 */
class RectilinearMesh : public Mesh {
	
	friend class Deleter<RectilinearMesh>;

	private:
	/**
	 * Constructor, takes a model and a name as parameter.
	 * The constructor is private, the "New" static function should
	 * be used to create Rectilinear Meshes.
	 *
	 * \param[in] mdl : model from which to initialize the RectilinearMesh.
	 */
	RectilinearMesh(const model::Mesh& mdl)
	: Mesh(mdl) {}
	
	/**
	 * Destructor.
	 */
	virtual ~RectilinearMesh() {}
	
	public:

	/**
	 * Creats an instance of RectilinearMesh given a model and a name.
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

		if(mdl.type() != model::MeshType::rectilinear) {
			return std::shared_ptr<RectilinearMesh>();
		}
		std::shared_ptr<RectilinearMesh> m = std::shared_ptr<RectilinearMesh>(
				new RectilinearMesh(mdl),
				Deleter<RectilinearMesh>());
		m->name_ = name;
		return m;
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
	 * Returns true is successful.
	 *
	 * \param[out] h : visit handle to create.
	 * \param[in] source : source to expose to visit.
	 * \param[in] iteration : iteration to expose to visit.
	 * \param[in] block : domain to expose to visit.
	 */
	bool ExposeVisItData(visit_handle* h, int source, int iteration, int block);
#endif

};

}
#endif
