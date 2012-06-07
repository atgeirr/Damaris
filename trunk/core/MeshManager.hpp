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
 * \file MeshManager.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_MESH_MANAGER_H
#define __DAMARIS_MESH_MANAGER_H

#include "xml/Model.hpp"
#include "data/Mesh.hpp"

namespace Damaris {

	/**
	 * MeshManager holds pointers to all Mesh objects.
	 * These objects are associated to a unique name and a unique id.
	 */
	class MeshManager : public Manager<Mesh> {

	public:
		/**
		 * This function initialize all mesh instances described in
		 * the XML model passed as parameter.
		 */
		static void Init(const Model::Data& mdl);

	private:
		/**
		 * Helper function to recursively navigate in the model.
		 */
		static void readMeshesInSubGroup(const Model::Group &g,
                        const std::string& groupName);
	};
}

#endif
