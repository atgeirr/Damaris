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

#ifndef __DAMARIS_CURVE_H
#define __DAMARIS_CURVE_H

#include "Damaris.h"

#ifdef HAVE_VISIT_ENABLED
	#include <VisItDataInterface_V2.h>
#endif
#include "damaris/util/Configurable.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/model/Model.hpp"

namespace damaris {

/**
 * The Curve object is used for describing a curve within
 * a metadata structure.
 */
class Curve : public Configurable<model::Curve> {

	friend class Manager<Curve>;
	friend class Deleter<Curve>;

	private:
		std::string name_; /*! Name of the mesh, copy from the model. */
		int id_; /*! id of the mesh, 
				provided when created by the MeshManager. */
		std::vector<std::shared_ptr<Variable> > coords_; /*! pointer to coordinate variables. */

	protected:
		/**
		 * Constructor, is protected (cannot be instanciated by outside,
		 * allows child classes).
		 */
		Curve(const model::Curve& mdl, const std::string& name)
		: Configurable<model::Curve>(mdl), name_(name) {}

		/**
		 * Destructor.
		 */
		virtual ~Curve() {}
			
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
		std::shared_ptr<Variable> GetCoord(unsigned int n);

		/**
		 * Returns the number of coordinates.
		 */
		unsigned int GetNumCoord() const {
			return GetModel().coord().size();
		}

#ifdef HAVE_VISIT_ENABLED
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool ExposeVisItMetaData(visit_handle md);
		
		/**
		 * Fills visit handle with metadata related to the Mesh.
		 */
		virtual bool ExposeVisItData(visit_handle* h, int iteration);
#endif

		/**
		 * Creates an instance of a child class of Mesh according to 
		 * the Curve model.
		 *
		 * \param[in] mdl : model from which to build a Curve instance.
		 */
		template<typename SUPER>
		static std::shared_ptr<SUPER> New(const model::Curve& mdl, 
						const std::string& name) {
			if(mdl.name().find("/") != std::string::npos) {
				CFGERROR("Curve " << mdl.name() << " cannot have a '/' character");
				return std::shared_ptr<SUPER>();
			}
			std::shared_ptr<Curve> c(new Curve(mdl,mdl.name()),
					Deleter<Curve>());
			c->name_ = name;
			return c;
		}
};

}
#endif
