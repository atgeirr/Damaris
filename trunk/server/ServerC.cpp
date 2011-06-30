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
 * \file ServerC.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * Contains the definition of functions for the Server object in C.
 */

#include "server/Server.hpp"

/** 
 * This object is declared extern and is associated to the Server object 
 * defined in Server.cpp.
 */
extern Damaris::Server* server;

extern "C" {

#include "server/Server.h"

	int DC_server(const char* configFile, int server_id)
	{
		std::string config_str(configFile);
		server = new Damaris::Server(&config_str,server_id);
		return server->run();
	}
}
