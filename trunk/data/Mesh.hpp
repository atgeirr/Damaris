/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file Mesh.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_MESH_H
#define __DAMARIS_MESH_H

#ifdef __ENABLE_VISIT
	#include <VisItDataInterface_V2.h>
#endif
#include "core/Configurable.hpp"
#include "core/Manager.hpp"
#include "xml/Model.hpp"

namespace Damaris {

/**
 * The Mesh object is used for describing a mesh within
 * a metadata structure.
 */
class Mesh : public Configurable<Model::Mesh> {

	friend class Manager<Mesh>;

	private:
		std::string name; /*! Name of the mesh, copy from the model. */
		int id;	/*! id of the mesh, provided when created by the MeshManager. */

	protected:
		/**
		 * Constructor, is protected (cannot be instanciated by outside,
		 * allows child classes).
		 */
		Mesh(const Model::Mesh& mdl, const std::string& name);

	public:

		virtual ~Mesh() {}

		/**
		 * Returns the name of the Mesh.
		 */
		const std::string& getName() const {
			return name;
		}

		/**
		 * Returns the id of the Mesh.
		 */
		int getID() const { return id; }

#ifdef __ENABLE_VISIT
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool exposeVisItMetaData(visit_handle md) const = 0;
		
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool exposeVisItData(visit_handle* h, int source, int iteration) const = 0;
#endif

		/**
		 * Creates an instance of a child class of Mesh according to the Mesh model.
		 */
		static Mesh* New(const Model::Mesh& mdl, const std::string& name);
};

}
#endif
