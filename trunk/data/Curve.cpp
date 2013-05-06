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
 * \file Curve.cpp
 * \date May 2013
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "data/Curve.hpp"
#include "core/VariableManager.hpp"
//#include "core/Environment.hpp"
#include "core/Debug.hpp"

namespace Damaris {

	Curve::Curve(const Model::Curve& mdl, const std::string &n)
		: Configurable<Model::Curve>(mdl)
	{
		name = n;
	}

	Curve* Curve::New(const Model::Curve& mdl, const std::string& name)
	{
		return new Curve(mdl,name);
	}

#ifdef __ENABLE_VISIT
	bool Curve::ExposeVisItMetaData(visit_handle md) const
	{
		visit_handle m1 = VISIT_INVALID_HANDLE;
		if(VisIt_CurveMetaData_alloc(&m1) == VISIT_OKAY)
		{
			VisIt_CurveMetaData_setName(m1, GetName().c_str());

			{ // check that all the coordinates have the same layout
				Model::Curve::coord_const_iterator it(model.coord().begin());

				Variable* vx = VariableManager::Search(it->name());
				if(vx == NULL) {
					ERROR("Unknown coordinate " << it->name());
					VisIt_CurveMetaData_free(m1);
					return VISIT_INVALID_HANDLE;
				}
				it++;

				Variable* vy = VariableManager::Search(it->name());
				if(vy == NULL) {
					ERROR("Unknown coordinate " << it->name());
					VisIt_CurveMetaData_free(m1);
					return VISIT_INVALID_HANDLE;
				}
				it++;

				Variable* vz = NULL;
				if(model.coord().size() == 3) {
					vz = VariableManager::Search(it->name());
					if(vz == NULL) {
						ERROR("Unknown coordinate " << it->name());
						VisIt_CurveMetaData_free(m1);
						return VISIT_INVALID_HANDLE;
					}
				}
			}

			Model::Curve::coord_const_iterator it(model.coord().begin());
			// the number of coordinates should be 2 or 3 (this condition is checked by
			// the xml loader)
			if(it->unit() != "#") VisIt_CurveMetaData_setXUnits(m1,it->unit().c_str());
			if(it->label() != "#") VisIt_CurveMetaData_setXLabel(m1,it->label().c_str());
			it++;

			if(it->unit() != "#") VisIt_CurveMetaData_setYUnits(m1,it->unit().c_str());
			if(it->label() != "#") VisIt_CurveMetaData_setYLabel(m1,it->label().c_str());
			it++;

			VisIt_SimulationMetaData_addCurve(md, m1);
			return true;
		}
		return false;
	}

	bool Curve::ExposeVisItData(visit_handle* h, int iteration) const
	{
		DBG("In Curve::exposeVisItData");
		// Allocates the VisIt handle
		if(VisIt_CurveData_alloc(h) != VISIT_ERROR) {
			visit_handle hxc, hyc = VISIT_INVALID_HANDLE;
			Variable *vx, *vy = NULL;

			Model::Curve::coord_const_iterator it(model.coord().begin());

			// Search for the X coordinate, checks that it has 1 dimenion
			vx = VariableManager::Search(it->name());
			if(vx == NULL) {
				CFGERROR("Undefined coordinate \""<< it->name() <<"\" for mesh \""
						<< GetName() << "\"");
				return false;
			}
			if(vx->GetLayout()->GetDimensions() != 1) {
				CFGERROR("Wrong number of dimensions for coordinate " << vx->GetName());
				return false;
			}
			it++;

			// Search for the Y coordinate, checks that it has 1 dimension
			vy = VariableManager::Search(it->name());
			if(vy == NULL) {
				CFGERROR("Undefined coordinate \""<< it->name() <<"\" for mesh \""
						<< GetName() << "\"");
				return false;
			}
			if(vy->GetLayout()->GetDimensions() != 1) {
				CFGERROR("Wrong number of dimensions for coordinate " << vy->GetName());
				return false;
			}
			it++;

			// Accessing chunk for X coordinate
			Chunk* c = vx->GetChunk(0,iteration,0);
			if(c != NULL) {
				if(VisIt_VariableData_alloc(&hxc) == VISIT_OKAY) {
					c->FillVisItDataHandle(hxc);
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
			c = vy->GetChunk(0,iteration,0);
			if(c != NULL) {
				if(VisIt_VariableData_alloc(&hyc) == VISIT_OKAY) {
					c->FillVisItDataHandle(hyc);
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
