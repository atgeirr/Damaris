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

#include "util/Debug.hpp"
#include "data/Mesh.hpp"
#include "data/VariableManager.hpp"

namespace damaris {

std::shared_ptr<Variable> Mesh::GetCoord(unsigned int n)
{
	if(coords_.size() == 0) { // first time access coordinates
		model::Mesh::coord_const_iterator 
			it(GetModel().coord().begin());
		for(; it != GetModel().coord().end(); it++) { 

			std::shared_ptr<Variable> v;
			std::string coordName = it->name();
			bool coordIsAbsolute = 
				(coordName.find("/") != std::string::npos);

			if(coordIsAbsolute) {
				v = VariableManager::Search(coordName);
			} else {
				std::vector<std::string> groups;
				boost::split(groups, GetName(), boost::is_any_of("/"));
				
				while(not v && groups.size() != 0) {
					
					groups.pop_back();
					std::string absoluteName;

					if(groups.size() != 0) {
						std::vector<std::string>::iterator it =
							groups.begin();
						absoluteName = *it;
						it++;
						for(; it != groups.end(); it++) {
							absoluteName += "/" + (*it);
						}
						absoluteName += "/" + coordName;
					} else {
						absoluteName = coordName;
					}
					v = VariableManager::Search(absoluteName);
				}
			}

			if(not v) {
				CFGERROR("Cannot find variable " << it->name()
					<< " to build mesh " << GetName());
			}
			coords_.push_back(v);
		}
	}

	if(n < coords_.size()) {
		return coords_[n];
	} else {
		return std::shared_ptr<Variable>();
	}
}

}
