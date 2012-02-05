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
 * \file Initiator.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_INITIATOR_H
#define __DAMARIS_INITIATOR_H

#include <string>
#include <vector>
#include <stdint.h>
#include <mpi.h>

#include "server/Server.hpp"
#include "client/Client.hpp"

namespace Damaris {

class Client;

/**
 * \class Initiator
 * This class is here so we can set the Client and Server constructors as private,
 * forcing the user to use the Client::New() and Server::New() functions.
 * It also provides the necessary tools for communicator splitting and process
 * creation.
 */
class Initiator {

	public:
		/**
		 * The start function loads the configuration file, then splits the
		 * communicator in a client part and a server part.
		 * If the calling process is a client, a Client object will be returned
		 * for the simulation to write in Damaris. If the calling process is
		 * server, a Server object will be created and the function won't return
		 * until the server has been killed. When it does return, the returned
		 * value is a NULL pointer.
		 *
		 * \param[in] configFile : name of the configuration file.
		 * \param[in] globalcomm : the MPI communicator to split.
		 */
		static Client* start(const std::string& configFile, MPI_Comm globalcomm);

}; // class Initiator

} // namespace Damaris

#endif
