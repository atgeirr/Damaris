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
 * \file Identified.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_IDENTIFIED_H
#define __DAMARIS_IDENTIFIED_H

namespace Damaris {

/**
 * This class is an abstract class representing a generic object
 * described by a name and an id.
 */
class Identified
{
	/**
	 * Returns the name of the object.
	 */
	virtual const std::string& getName() const = 0;

	/**
	 * Returns the id of the object.
	 */
	virtual int getID() const = 0;
};

}

#endif
