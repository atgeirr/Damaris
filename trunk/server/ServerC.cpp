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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include <mpi.h>

#include "server/Initiator.hpp"
#include "server/Server.hpp"

extern Damaris::Server* server; // defined in Server.cpp
extern Damaris::Client* client; // defined in ClientC.cpp

extern "C" {

#include "server/Server.h"

	int DC_server(const char* configFile, int server_id)
	{
		std::string config_str(configFile);
		server = Damaris::Server::New(config_str,server_id);
		server->run();
		delete server;
		return 0;
	}

	int DC_mpi_start(const char* configFile, MPI_Comm globalcomm)
	{
		client = Damaris::Initiator::start(std::string(configFile),globalcomm);
		return (client != NULL);
	}
}
