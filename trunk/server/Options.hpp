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
 * \brief Program options parsing
 * \author Matthieu Dorier
 * \version 0.1
 */
#ifndef __DAMARIS_OPTIONS_H
#define __DAMARIS_OPTIONS_H

#include <string>

#include "common/Configuration.hpp"

namespace Damaris {

class Options {
	private:
		std::string* configFile;
		Configuration* config;
		int id;
	public:
		Options(int argc, char** argv);
		std::string* getConfigFile();
		Configuration* getConfiguration();
		int getID();
};
}

#endif
