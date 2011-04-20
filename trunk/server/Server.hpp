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
 * \brief Damaris Server object
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
#include "common/BehaviorManager.hpp"
#include "common/MetadataManager.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	class Server {
	public:
		Server(std::string *config, int id);
		~Server();
		void run();
		void stop();
	private:
		bool needStop;
		Configuration *config;
		message_queue *msgQueue;
		managed_shared_memory *segment;
		MetadataManager *metadataManager;
		BehaviorManager *behaviorManager;
		
		void processMessage(Message* msg);
	}; // class Server
	
	
} // namespace Damaris

#endif
