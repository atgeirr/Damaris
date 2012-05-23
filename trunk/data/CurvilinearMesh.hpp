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
 * \file CurvilinearMesh.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_CURVILINEAR_MESH_H
#define __DAMARIS_CURVILINEAR_MESH_H

#include "data/Mesh.hpp"

namespace Damaris {

/**
 */
class CurvilinearMesh : public Mesh {

	private:
		CurvilinearMesh(const Model::Mesh& mdl, const std::string& name);
	
	public:

		static CurvilinearMesh* New(const Model::Mesh& mdl, const std::string& name);

#ifdef __ENABLE_VISIT
		bool exposeVisItMetaData(visit_handle md) const;
		
		bool exposeVisItData(visit_handle* h, int source, int iteration) const;
#endif

};

}
#endif
