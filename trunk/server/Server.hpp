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
 * \file Server.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <string>
#include <mpi.h>

#include "client/Client.hpp"
#include "common/Message.hpp"

namespace Damaris {

	class Server : public Client {
		public:
			/**
			 * \brief Constructor.
			 * Used when starting a server from an external program
			 * 
			 * \param[in] configFile : name of the configuration file.
			 * \param[in] id : identifier for this server.
			 */
			Server(const std::string &configFile, int id);

			/**
			 * \brief Constructor. 
			 * Used by the standalone server program, which pre-initializes 
			 * a configuration through command line arguments, 
			 * and a pre-initialized environment 
			 *
			 * \param[in] config : a fully initialized Configuration object.
			 */
			Server(Configuration* config);

			/**
			 * \brief Destructor 
			 * Delete all related objects (configuration, environment, shared memory objects).
			 */
			~Server();

			/** 
			 * \brief Enters in the server main loop.
			 */
			int run();

			/** 
			 * \brief Forces the server to stop after the next iteration of the main loop. 
			 */
			void stop();

		private:

			int needStop; /*!< indicates wether the server has to 
					exit the main loop at the next iteration. */

			/** 
			 * Initializes everything (called by constructors).
			 */
			virtual void init();

			/** 
			 * This function processes an incoming message (no way?!).
			 * \see Damaris::Message
			 */
			void processMessage(Message* msg);

			/**
			 * This function is called when receiving a message of type MSG_INT;
			 * See the #define statements in Message.hpp.
			 */
			void processInternalSignal(int32_t obj);

		public:
			/**
			 * \brief Writes a full variable.
			 * \see Client::write
			 */
			int write(const std::string & varname, int32_t iteration, const void* data);

			/**
			 * \brief Writes a chunk of a variable.
			 * \see Client::chunk_write
			 */
			int chunk_write(chunk_h chunkh, const std::string & varname,
					int32_t iteration, const void* data);

			/**
			 * \brief Sends an event.
			 * \see Client::signal
			 */
			int signal(const std::string & signame, int32_t iteration);

			/**
			 * \brief Allocate a buffer directly in shared memory for future writing.
			 * \see Client::alloc
			 */
			void* alloc(const std::string & varname, int32_t iteration);

			/** 
			 * \brief Commit a variable.
			 * \see Client::commit
			 */
			int commit(const std::string & varname, int32_t iteration);

			/**
			 * \see Client::kill_server().
			 */
			int kill_server();
	}; // class Server

	/**
	 * \brief Starts the entities according to the configuration file.
	 * If the process is a client, a valid pointer over a Client object is returned.
	 * If the process is a server, this function starts the server loop and
	 * blocks, returning only when the server is kill through the kill_server function
	 * of clients, and the returned value is a NULL pointer.
	 * If a problem occurs in this function, the program will abort.
	 */
	Client* start_mpi_entity(const std::string& config, MPI_Comm globalcomm);

} // namespace Damaris

#endif
