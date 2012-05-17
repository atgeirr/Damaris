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
 * \file Options.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_OPTIONS_H
#define __DAMARIS_OPTIONS_H

#include <string>

#include "core/Environment.hpp"

namespace Damaris {

/**
 * The Options object can be used to parse the command line.
 */
class Options {
	private:
		std::string configFile; /*!< Name of the configuration file. */

	public:
		/**
		 * The constructor takes the same parameters than a usual program. 
		 */
		Options(int argc, char** argv);
		/**
		 * Get the name of the configuration file.
		 */
		const std::string& getConfigFile();
};
}

#endif
