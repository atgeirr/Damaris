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
 * \file Server.h
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * Defines the C server interface.
 */
#ifndef __DAMARIS_SERVER_C_H
#define __DAMARIS_SERVER_C_H
#ifdef __ENABLE_MPI
	#include <mpi.h>
#endif
/**
 * Starts a server (blocks in the server's mainloop).
 * \param[in] configFile : name of the XML configuration file.
 * \param[in] server_id : an identifier for this server.
 */
int DC_server(const char* configFile, int server_id);
#ifdef __ENABLE_MPI
/**
 * Start clients and servers as MPI entities.
 * \param[in] configFile : name of the configuration file to use.
 * \param[in] oldcomm : communicator to split.
 * \param[out] newcomm : communicator gathering clients.
 * \return a positive integer for clients, 0 for servers (blocks in the function
 * until the server is killed).
 */
int DC_start_mpi_entity(const char* configFile, MPI_Comm globalcomm);
#endif
#endif
