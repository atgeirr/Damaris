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
#ifndef __DAMARIS_CURVE_MANAGER_H
#define __DAMARIS_CURVE_MANAGER_H

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/data/Curve.hpp"

namespace damaris {

/**
 * CurveManager holds pointers to all Curve objects.
 * These objects are associated to a unique name and a unique id.
 */
class CurveManager : public Manager<Curve> {

	public:
	/**
	 * This function initialize all mesh instances described in
	 * the XML model passed as parameter.
	 * 
	 * \param[in] mdl : model from which to fill the CurveManager.
	 */
	static void Init(const model::Data& mdl);

	private:
			
	/**
	 * Helper function to recursively navigate in the model.
	 * 
	 * \param[in] g : subgroup to go through.
	 * \param[in] groupName : name of the current group (full name).
	 */
	static void ReadCurvesInSubGroup(const model::Group &g,
                        const std::string& groupName);
};

}

#endif
