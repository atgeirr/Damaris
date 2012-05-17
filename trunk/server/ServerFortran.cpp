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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 * Contains the definition of functions for the Server in Fortran.
 */
#ifdef __ENABLE_FORTRAN
#include "core/FCMangle.h"

#ifndef FC_FUNC_GLOBAL
#define FC_FUNC_GLOBAL(name,NAME) name##_
#endif

#ifndef FC_FUNC_GLOBAL_
#define FC_FUNC_GLOBAL_(name,NAME) name##_
#endif

#include <mpi.h>

#include "server/Initiator.hpp"
#include "server/Server.hpp"

extern Damaris::Server *server;
extern Damaris::Client *client;

extern "C" {

void FC_FUNC_GLOBAL(df_server,DF_SERVER)
	(char* configFile_f, int32_t* server_id_f, int32_t* ierr_f, int32_t configFile_size)
	{
		std::string config_str(configFile_f,configFile_size);
		server = Damaris::Server::New(config_str,*server_id_f);
		*ierr_f = server->run();
		delete server;
	}

void FC_FUNC_GLOBAL(df_mpi_start,DF_START_MPI_ENTITY)
	(char* configFile_f, MPI_Fint* globalcomm, int* result, int configsize)
	{
		MPI_Comm oc = MPI_Comm_f2c(*globalcomm);
		client = Damaris::Initiator::start(std::string(configFile_f,configsize),oc);
		*result = (client != NULL) ? 1 : 0;
	}
}
#endif
