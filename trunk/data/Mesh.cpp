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
 * \file Mesh.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "data/Mesh.hpp"
#include "core/VariableManager.hpp"
#include "data/CurvilinearMesh.hpp"
#include "data/RectilinearMesh.hpp"
#include "core/Debug.hpp"

namespace Damaris {

	Mesh::Mesh(const Model::Mesh& mdl, const std::string &n)
		: Configurable<Model::Mesh>(mdl)
	{
		name = n;
	}

	Mesh* Mesh::New(const Model::Mesh& mdl, const std::string& name)
	{
		switch(mdl.type()) {
			case Model::MeshType::rectilinear :
				return RectilinearMesh::New(mdl,name);
			case Model::MeshType::curvilinear :
				return CurvilinearMesh::New(mdl,name);
			case Model::MeshType::unstructured :
			case Model::MeshType::point :
			case Model::MeshType::csg :
			case Model::MeshType::amr :
			case Model::MeshType::unknown :
			default: break;
		}
		ERROR("Mesh type " << mdl.type() << " is not implemented.");
		return NULL;
	}

	int Mesh::CountLocalBlocks(int iteration) const
	{
		int c = 0;
		Model::Mesh::coord_const_iterator it(model.coord().begin());

		Variable* vx = VariableManager::Search(it->name());
		if(vx == NULL) return 0;
		c = vx->CountLocalBlocks(iteration);
		it++;

		Variable* vy = VariableManager::Search(it->name());
		if(vy == NULL) return 0;
		if(c != vy->CountLocalBlocks(iteration)) {
			ERROR("Inconsistent number of blocks between coordinates");
			return 0;
		}
		it++;

		if(model.coord().size() == 2)
			return c;

		Variable* vz = VariableManager::Search(it->name());
		if(vz == NULL) return 0;
		if(c != vz->CountLocalBlocks(iteration)) {
			ERROR("Inconsistent number of blocks between coordinates");
			return 0;
		}
		return c;
	}

	int Mesh::CountTotalBlocks(int iteration) const
	{
		int c = 0;
		Model::Mesh::coord_const_iterator it(model.coord().begin());

		Variable* vx = VariableManager::Search(it->name());
		if(vx == NULL) return 0;
		c = vx->CountTotalBlocks(iteration);
		it++;

		Variable* vy = VariableManager::Search(it->name());
		if(vy == NULL) return 0;
		if(c != vy->CountTotalBlocks(iteration)) {
			ERROR("Inconsistent number of blocks between coordinates");
			return 0;
		}
		it++;

		if(model.coord().size() == 2)
			return c;

		Variable* vz = VariableManager::Search(it->name());
		if(vz == NULL) return 0;
		if(c != vz->CountTotalBlocks(iteration)) {
			ERROR("Inconsistent number of blocks between coordinates");
			return 0;
		}
		return c;
	}

}
