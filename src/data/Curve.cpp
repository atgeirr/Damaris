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

#include "data/Curve.hpp"
#include "data/VariableManager.hpp"
#include "util/Debug.hpp"

namespace damaris {

shared_ptr<Variable> Curve::GetCoord(unsigned int n)
{
	if(coords_.size() == 0) { // first time access coordinates
		model::Curve::coord_const_iterator
			it(GetModel().coord().begin());
		for(; it != GetModel().coord().end(); it++) {
			
			shared_ptr<Variable> v;
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
					<< " to build curve " << GetName());
			}

			coords_.push_back(v);
		}
	}

	if(n < coords_.size()) {
		return coords_[n];
	} else {
		return shared_ptr<Variable>();
	}
}


#ifdef HAVE_VISIT_ENABLED
bool Curve::ExposeVisItMetaData(visit_handle md) 
{
	visit_handle m1 = VISIT_INVALID_HANDLE;
	if(VisIt_CurveMetaData_alloc(&m1) == VISIT_OKAY)
	{
		VisIt_CurveMetaData_setName(m1, GetName().c_str());
		
		// check that the coordinate exist
		shared_ptr<Variable> vx = GetCoord(0);
		shared_ptr<Variable> vy = GetCoord(1);
		if((not vx) || (not vy)) {
			VisIt_CurveMetaData_free(m1);
			return false;
		}

		model::Curve::coord_const_iterator 
			it(GetModel().coord().begin());
		// the number of coordinates should be 2 or 3 
		// (this condition is checked by the xml loader)
		if(it->unit() != "#") 
			VisIt_CurveMetaData_setXUnits(m1,it->unit().c_str());
		if(it->label() != "#") 
			VisIt_CurveMetaData_setXLabel(m1,it->label().c_str());
		it++;

		if(it->unit() != "#") 
			VisIt_CurveMetaData_setYUnits(m1,it->unit().c_str());
		if(it->label() != "#") 
			VisIt_CurveMetaData_setYLabel(m1,it->label().c_str());
		it++;

		VisIt_SimulationMetaData_addCurve(md, m1);
		return true;
	}
	return false;
}

bool Curve::ExposeVisItData(visit_handle* h, int iteration)
{
	DBG("In Curve::exposeVisItData");
	// Allocates the VisIt handle
	if(VisIt_CurveData_alloc(h) != VISIT_ERROR) {
		visit_handle hxc, hyc = VISIT_INVALID_HANDLE;
		shared_ptr<Variable> vx, vy;
		
		//model::Curve::coord_const_iterator 
		//	it(GetModel().coord().begin());

		// Search for the X coordinate, checks that it has 1 dimenion
		vx = GetCoord(0);
		//VariableManager::Search(it->name());
		if(not vx) {
			//CFGERROR("Undefined coordinate \""
			//	<< it->name() <<"\" for mesh \""
			//	<< GetName() << "\"");
			return false;
		}
		if(vx->GetLayout()->GetDimensions() != 1) {
			CFGERROR("Wrong number of dimensions for coordinate " 
				<< vx->GetName());
			return false;
		}
		//it++;

		// Search for the Y coordinate, checks that it has 1 dimension
		vy = GetCoord(1); //VariableManager::Search(it->name());
		if(not vy) {
			//CFGERROR("Undefined coordinate \""
			//	<< it->name() <<"\" for mesh \""
			//	<< GetName() << "\"");
			return false;
		}
		if(vy->GetLayout()->GetDimensions() != 1) {
			CFGERROR("Wrong number of dimensions for coordinate " 
				<< vy->GetName());
			return false;
		}
		//it++;

		// Accessing chunk for X coordinate
		shared_ptr<Block> b = vx->GetBlock(0,iteration,0);
		if(b) {
			if(VisIt_VariableData_alloc(&hxc) == VISIT_OKAY) {
				b->FillVisItDataHandle(hxc);
			} else {
				ERROR("While allocating data handle");
				return false;
			}
		} else {
			VisIt_CurveData_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}

		// Accessing chunk for Y coordinate
		b = vy->GetBlock(0,iteration,0);
		if(b) {
			if(VisIt_VariableData_alloc(&hyc) == VISIT_OKAY) {
				b->FillVisItDataHandle(hyc);
			} else {
				ERROR("While allocating data handle");
				VisIt_VariableData_free(hxc);
				VisIt_CurveData_free(*h);
				*h = VISIT_INVALID_HANDLE;
				return false;
			}
		} else {
			VisIt_VariableData_free(hxc);
			VisIt_CurveData_free(*h);
			*h = VISIT_INVALID_HANDLE;
			return false;
		}

		VisIt_CurveData_setCoordsXY(*h, hxc, hyc);
	}

	return (*h != VISIT_INVALID_HANDLE);
}

#endif
}
