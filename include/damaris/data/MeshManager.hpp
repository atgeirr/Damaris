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

#ifndef __DAMARIS_MESH_MANAGER_H
#define __DAMARIS_MESH_MANAGER_H

#include <string>

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/data/Mesh.hpp"

namespace damaris {

/**
 * MeshManager holds pointers to all Meshes.
 * These Meshes can be retrieved by their name or by a unique ID.
 */
class MeshManager : public Manager<Mesh> {

	public:
	/**
	 * Initializes the VariableManager given an XML model.
	 * Goes through all the variables described in the model,
	 * creates and stores them.
	 *
	 * \param[in] mdl : model from which to initialize the MeshManager.
	 */
	static void Init(const model::Data& mdl);

	private:
	
	/**
	 * Go through a Model::Group instance and read Meshes recursively. 
	 *
	 * \param[in] g : a model::Group to parse.
	 * \param[in] groupName : the parent group absolute name.
	 */
	static void ReadMeshesInSubGroup(const model::Group &g,
		const std::string& groupName);

};

}

#endif
