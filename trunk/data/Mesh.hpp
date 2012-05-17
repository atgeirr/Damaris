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
namespace Viz {

/**
 * The Mesh object is used for describing a mesh within
 * a metadata structure.
 */
class Mesh : public Configurable<Model::Mesh> {

	private:
		std::string name;
		int id;

	protected:
		Mesh(const Model::Mesh& mdl);
		
	public:

		const std::string& getName() const {
			return name;
		}

		int getID() const { return id; }

#ifdef __ENABLE_VISIT
		virtual bool exposeVisItMetaData(visit_handle md) const = 0;
		
		virtual bool exposeVisItData() const = 0;
#endif

		static Mesh* New(const Model::Mesh& mdl);
};

}
}
#endif
