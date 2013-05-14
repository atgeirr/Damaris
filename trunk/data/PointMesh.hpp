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
 * \file PointMesh.hpp
 * \date May 2013
 * \author Matthieu Dorier
 * \version 0.8
 */
#ifndef __DAMARIS_POINT_MESH_H
#define __DAMARIS_POINT_MESH_H

#include "data/Mesh.hpp"

namespace Damaris {

/**
 * PointMesh is a particular type of Mesh.
 */
class PointMesh : public Mesh {

	private:
		/**
		 * Constructor, takes a model and a name as parameter.
		 * The constructor is private, the "New" static function should
		 * be used to create Rectilinear Meshes.
		 */
		PointMesh(const Model::Mesh& mdl, const std::string& name);
	
	public:

		/**
		 * Creats an instance of RectilinearMesh given a model and a name.
		 * Do some additional checking before creating it. If these verifications fail,
		 * return NULL.
		 */
		static PointMesh* New(const Model::Mesh& mdl, const std::string& name);

#ifdef __ENABLE_VISIT
		/**
		 * Fills the visit handle with Metadata related to this Mesh.
		 * Returns true in case of success.
		 */
		bool ExposeVisItMetaData(visit_handle md) const;
		
		/**	
		 * Fills the visit handle with the data related to this Mesh.
		 * Returns true is successful.
		 */
		bool ExposeVisItData(visit_handle* h, int source, int iteration, int block) const;
#endif

};

}
#endif
