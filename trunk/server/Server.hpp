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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <string>
#include <mpi.h>

#include "memory/Message.hpp"
#include "core/Process.hpp"
#include "comm/MPILayer.hpp"
#include "comm/CollectiveRPC.hpp"
#include "xml/BcastXML.hpp"
#include "server/Initiator.hpp"

namespace Damaris {

	class Server {
		friend class Initiator;

		private:
			Process* process;
			MPILayer<CollectiveRPC<void (*)(int)>::rpc_msg>* commLayer;
			CollectiveRPC<void (*)(int)>* rpcLayer;

			/**
			 * \brief Constructor.
			 * Used when starting a server from an external program.
			 * This constructor is private, only the Initiator can call it.
			 * 
			 * \param[in] p : an initialized Process instance.
			 */
			Server(Process* p);

		public:

			/**
			 * This static function is the normal way of retrieving
			 * an instance over a Server object.
			 * \param[in] cfgfile : name of the configuration file.
			 * \param[in] id : id of the process.
			 */
			static Server* New(std::auto_ptr<Model::Simulation> mdl, int32_t id);

			/**
			 * \brief Destructor 
			 * Delete all related objects 
			 * (configuration, environment, shared memory objects).
			 */
			~Server();

			/** 
			 * \brief Enters in the server main loop.
			 */
			int run();

			/** 
			 * Forces the server to stop after the next iteration of the main loop.
			 * Does not remove the shared memory structures (the server can run again if
			 * necessary).
			 */
			void stop();

		private:

			int needStop; /*!< indicates wether the server has to 
					exit the main loop at the next iteration. 
					This variable is initialized to the number of clients
				 	that have to send a kill signal before exiting. */

			/** 
			 * This function processes an incoming message (no way?!).
			 * \see Damaris::Message
			 */
			void processMessage(const Message& msg);

			/**
			 * This function is called when receiving a message of type MSG_INT;
			 * See the enumration statements in Message.hpp.
			 */
			void processInternalSignal(int32_t obj, int iteration, int source);

	}; // class Server

} // namespace Damaris

#endif
