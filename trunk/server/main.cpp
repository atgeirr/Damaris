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
 * \file main.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 * This file contains the main function for the server in standalone mode.
 */

#include <iostream>
#include <list>
#include <string>
#include <signal.h>
#include <mpi.h>

#include "server/Options.hpp"
#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "server/Server.hpp"

/**
 * Catches Ctr^C (kill signals)
 * in order to clean everything before exiting.
 * \param[in] sig : the signal to catch.
 */
static void sighandler(int sig);

/**
 * A global instance of server (there should be only one
 * so this one is set as extern to refer to the one defined
 * in Server.cpp)
 */
extern Damaris::Server *server;
/**
 * Main function for the standalone mode server.
 * \param[in] argc : argc
 * \param[in] argv : argv
 **/
int main(int argc, char** argv)
{
	MPI_Init(&argc,&argv);
	INFO("Parsing program options");
	/* The Options object is used to parse the command line arguments */
	Damaris::Options opt(argc,argv);

	/* Attaching sighandler to signals */
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT,  &sighandler);
	
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	INFO("Initializing server");
	/* Initializing the server with a Configuration object 
	   pre-initialized by the Options object */
	server = Damaris::Server::New(opt.getConfigFile(),id);

	INFO("Setting communicators");
	Damaris::Environment::setGlobalComm(MPI_COMM_WORLD);
	Damaris::Environment::setEntityComm(MPI_COMM_WORLD);

	INFO("Starting server");
	/* Starts the server */
	server->run();
	
	/* If we go here, it means that a requests has been sent
	   to the server for stoping it. This is the normal way
	   of doing. */
	INFO("Correctly terminating server\n");
	delete server;
	
	MPI_Finalize();
	return 0;
}

static void sighandler(int sig)
{
	INFO("Kill signal caught, server will terminate");
	if(server != NULL) {
		server->stop();
		delete server;
	}
	exit(0);
}
