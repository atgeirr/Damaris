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
 * \file ProcInfo.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_PROC_INFO_H
#define __DAMARIS_PROC_INFO_H

namespace Damaris {

/**
 * The ProcInfo namespace contains functions that help retrieving
 * information about the processor, such as a unique ID.
 */
namespace ProcInfo {

	/**
	 * Returns a unique node identifier based on a hash of its name.
	 */
	int GetNodeID();

}

} // namespace Damaris

#endif
