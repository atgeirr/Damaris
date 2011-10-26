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

#ifdef __ENABLE_MPI
	#include <mpi.h>
#endif

#include "server/Server.hpp"

extern Damaris::Server *server;
extern Damaris::Client *client;

extern "C" {

void FC_FUNC_GLOBAL(df_server,DF_SERVER)
	(char* configFile_f, int32_t* server_id_f, int32_t* ierr_f, int32_t configFile_size)
	{
		std::string config_str(configFile_f,configFile_size);
		server = new Damaris::Server(config_str,*server_id_f);
		*ierr_f = server->run();
	}
}

#ifdef __ENABLE_MPI
void FC_FUNC_GLOBAL(df_start_mpi_entity,DF_START_MPI_ENTITY)
	(char* configFile_f, MPI_Fint* newcomm,	int* newrank, 
	 int* newsize, int* result, int configsize)
	{
		MPI_Comm c = MPI_Comm_f2c(*newcomm);
		client = Damaris::start_mpi_entity(std::string(configFile_f,configsize), 
				newcomm, newrank, newsize);
		*newcomm = MPI_Comm_c2f(c);
		*result = (client != NULL) ? 1 : 0;
	}
#else
void FC_FUNC_GLOBAL(df_start_mpi_entity,DF_START_MPI_ENTITY)
        (char* configFile_f, void* newcomm, int* newrank,
	 int* newsize, int* result, int configsize)
        {
                Damaris::start_mpi_entity(std::string(configFile_f,configsize),
                                newcomm, newrank, newsize);
		*result = -1;
        }
#endif

#endif
