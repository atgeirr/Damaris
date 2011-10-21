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
 * \file ServerFortran.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * Contains the definition of functions for the Server in Fortran.
 */
#ifdef __ENABLE_FORTRAN
#include "common/FCMangle.h"

#ifndef FC_FUNC_GLOBAL
#define FC_FUNC_GLOBAL(name,NAME) name##_
#endif

#ifndef FC_FUNC_GLOBAL_
#define FC_FUNC_GLOBAL_(name,NAME) name##_
#endif

#include "server/Server.hpp"

/** 
 * This object is declared extern and is associated to the Server object 
 * defined in Server.cpp.
 */
extern Damaris::Server *server;

extern "C" {

void FC_FUNC_GLOBAL(df_server,DF_SERVER)
	(char* configFile_f, int32_t* server_id_f, int32_t* ierr_f, int32_t configFile_size)
	{
		std::string config_str(configFile_f,configFile_size);
		server = new Damaris::Server(config_str,*server_id_f);
		*ierr_f = server->run();
	}
}
#endif
