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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * 
 * Contains the definition of the Server object. The Server is the code
 * running on dedicated cores.
 */
#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <string>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "common/Configuration.hpp"
#include "common/Environment.hpp"
#include "common/Message.hpp"
#include "common/ActionsManager.hpp"
#include "common/MetadataManager.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
class Server {
	public:
		/**
		 * \brief Constructor.
		 * Used when starting a server from an external program
		 * 
		 * \param[in] configFile : name of the configuration file.
		 * \param[in] id : identifier for this server.
		 */
		Server(std::string *configFile, int id);

		/**
		 * \brief Constructor. 
		 * Used by the standalone server program, which pre-initializes 
		 * a configuration through command line arguments, 
		 * and a pre-initialized environment 
		 *
		 * \param[in] config : a fully initialized Configuration object.
		 * \param[in] env : a fully initialized Environment object.
		 */
		Server(Configuration* config, Environment* env);

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

		bool needStop; /*!< indicates wether the server has to exit the main loop at the next iteration */

		Configuration *config; /*!< This is the configuration object initialized with the external file. */
		Environment *env; /*!< This is the pointer to the Environment object. */
		
		/** 
		 * Pointer to the message queue, this queue is used
		 * to send messages related to incoming variables and events.
		 */
		message_queue *msgQueue;

		/**
		 * Pointer to the shared memory segment, used for writing variables. 
		 */
		managed_shared_memory *segment;

		/**
		 * This is the entry point to the metadata layer of Damaris. 
		 * \see Damaris::MetadataManager
		 */
		MetadataManager *metadataManager;
		
		/** 
		 * This object contains all the user-defined actions. 
		 * \see Damaris::ActionsManager
		 */
		ActionsManager *actionsManager;
		
		/** 
		 * Initializes everything (called by constructors).
		 */
		void init();

		/** 
		 * This function processes an incoming message (no way?!).
		 * \see Damaris::Message
		 */
		void processMessage(Message* msg);
}; // class Server
	
} // namespace Damaris

#endif
