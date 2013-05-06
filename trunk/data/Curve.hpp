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
 * \file Curve.hpp
 * \date May 2013
 * \author Matthieu Dorier
 * \version 0.8
 */
#ifndef __DAMARIS_CURVE_H
#define __DAMARIS_CURVE_H

#ifdef __ENABLE_VISIT
	#include <VisItDataInterface_V2.h>
#endif
#include "core/Configurable.hpp"
#include "core/Manager.hpp"
#include "xml/Model.hpp"

namespace Damaris {

/**
 * The Curve object is used for describing a curve within
 * a metadata structure.
 */
class Curve : public Configurable<Model::Curve> {

	friend class Manager<Curve>;

	private:
		std::string name; /*! Name of the mesh, copy from the model. */
		int id;	/*! id of the mesh, provided when created by the MeshManager. */

	protected:
		/**
		 * Constructor, is protected (cannot be instanciated by outside,
		 * allows child classes).
		 */
		Curve(const Model::Curve& mdl, const std::string& name);

	public:

		virtual ~Curve() {}

		/**
		 * Returns the name of the Mesh.
		 */
		const std::string& GetName() const {
			return name;
		}

		/**
		 * Returns the id of the Mesh.
		 */
		int GetID() const { return id; }

#ifdef __ENABLE_VISIT
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool ExposeVisItMetaData(visit_handle md) const;
		
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool ExposeVisItData(visit_handle* h, int iteration) const;
#endif

		/**
		 * Creates an instance of a child class of Mesh according to 
		 * the Curve model.
		 */
		static Curve* New(const Model::Curve& mdl, 
				  const std::string& name);
};

}
#endif
