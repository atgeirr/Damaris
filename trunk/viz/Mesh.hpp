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

#include "common/Identified.hpp"
#include "common/Configurable.hpp"
#include "common/Manager.hpp"
#include "xml/Model.hpp"

namespace Damaris {
namespace Viz {

/**
 * The Mesh object is used for describing a mesh within
 * a metadata structure.
 */
class Mesh : public Identified, public Configurable<Model::Mesh> {

	friend class Manager<Mesh,Model::Mesh*>;
	
	private:
		int id;
		Mesh(const Model::Mesh& mdl);
		
	public:

		const std::string& getName() const {
			return model.name();
		}

		int getID() const {
			return id;
		}
};

}
}
#endif
