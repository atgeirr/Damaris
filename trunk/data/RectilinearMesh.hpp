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
 * \file RectilinearMesh.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_RECTILINEAR_MESH_H
#define __DAMARIS_RECTILINEAR_MESH_H

#include "data/Mesh.hpp"

namespace Damaris {
namespace Viz {

/**
 */
class RectilinearMesh : public Mesh {
	friend class Mesh;

	private:
		RectilinearMesh(const Model::Mesh& mdl);
	
	public:

#ifdef __ENABLE_VISIT
		bool exposeVisItMetaData(visit_handle md) const;
		
		bool exposeVisItData() const;
#endif

};

}
}
#endif
