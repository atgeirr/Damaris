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
 */
#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <string>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "common/Configuration.hpp"
#include "common/Message.hpp"
#include "common/ActionsManager.hpp"
#include "common/MetadataManager.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
class Server {
	public:
		/* This constructor is used when starting a server
		   from an external program */
		Server(std::string *configFile, int id);
		/* This constructor is used by the standalone server
		   program, which pre-initializes a configuration through
		   command lines arguments, and a pre-initialized environment */
		Server(Configuration* config, Environment* env);
		/* Object destructor */
		~Server();
		/* Enters in the server main loop */
		int run();
		/* Forces the server to stop */
		void stop();
	private:
		/* needStop indicated if the server has to exit the
		   main loop at the next iteration */
		bool needStop;
		/* This is the configuration object initialized with the external file */
		Configuration *config;
		/* This is the environment pointer */
		Environment *env;
		/* Pointer to the message queue, this queue is used
		   to send messages related to incoming variables and
		   events */
		message_queue *msgQueue;
		/* Pointer to the shared memory segment, used to
		   write variables */
		managed_shared_memory *segment;
		/* This is the entry to the metadata layer of Damaris */
		MetadataManager *metadataManager;
		/* This object contains all the user-defined actions */
		ActionsManager *actionsManager;
		/* The init function initializes everything (called by
		   constructors */
		void init();
		/* This function processes an incoming message (no way ???) */
		void processMessage(Message* msg);
}; // class Server
	
} // namespace Damaris

#endif
