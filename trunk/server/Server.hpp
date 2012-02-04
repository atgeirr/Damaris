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

#include "common/Message.hpp"
#include "common/Process.hpp"
#include "server/Initiator.hpp"

namespace Damaris {

	class Server {
		friend class Initiator;

		private:
			Process* process;

			/**
			 * \brief Constructor.
			 * Used when starting a server from an external program
			 * 
			 * \param[in] configFile : name of the configuration file.
			 * \param[in] id : identifier for this server.
			 */
			Server(Process* p);

		public:

			static Server* New(const std::string& cfgfile, int32_t id);

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
//			int write(const std::string & varname, int32_t iteration, const void* data);

			/**
			 * \brief Writes a chunk of a variable.
			 * \see Client::chunk_write
			 */
//			int chunk_write(chunk_h chunkh, const std::string & varname,
//					int32_t iteration, const void* data);

			/**
			 * \brief Sends an event.
			 * \see Client::signal
			 */
//			int signal(const std::string & signame, int32_t iteration);

			/**
			 * \brief Allocate a buffer directly in shared memory for future writing.
			 * \see Client::alloc
			 */
//			void* alloc(const std::string & varname, int32_t iteration);

			/** 
			 * \brief Commit a variable.
			 * \see Client::commit
			 */
//			int commit(const std::string & varname, int32_t iteration);

			/**
			 * \see Client::kill_server().
			 */
//			int kill_server();
	}; // class Server

} // namespace Damaris

#endif
