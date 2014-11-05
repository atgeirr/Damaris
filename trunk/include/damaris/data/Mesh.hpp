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

#include "damaris/util/Pointers.hpp"
#include "damaris/util/TypeWrapper.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Configurable.hpp"

#include "damaris/model/Model.hpp"

namespace damaris {
	
USING_POINTERS;

/**
 * The Mesh object is used for describing a mesh within a metadata structure.
 */
class Mesh : public Configurable<model::Mesh> {

	friend class Deleter<Mesh>;
	friend class Manager<Mesh>;

	protected:
		int id_; /*! id of the mesh, provided by the MeshManager. */
		std::string name_; /*! Name of the mesh, copy from the model. */
		std::vector<shared_ptr<Variable> > coords_; /*! pointer to coordinate variables. */

	protected:
		
		/**
		 * Constructor, is protected (cannot be instanciated by outside,
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
		 * Will search for the coordinates only once and store the
		 * the result for later calls.
		 *
		 * \param[in] n : index of the coordinate.
		 */
		shared_ptr<Variable> GetCoord(unsigned int n);

		/**
		 * Returns the number of coordinates.
		 */
		unsigned int GetNumCoord() const {
			return GetModel().coord().size();
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
};

}
#endif
