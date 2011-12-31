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
 * \file SecondaryServer.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include <iostream>
#include <list>
#include "common/Debug.hpp"
#include "common/Environment.hpp"
#include "common/SharedMemorySegment.hpp"
#include "common/Configuration.hpp"
#include "server/Server.hpp"
#include "server/SecondaryServer.hpp"
#include "common/Message.hpp"
#include "common/ShmChunk.hpp"
#include "common/Layout.hpp"
#include "common/SharedMemory.hpp"

namespace Damaris {

/* constructor for embedded mode */
SecondaryServer::SecondaryServer(const std::string &cf, int id)
: Server(cf,id)
{
}

/* constructor for standalone mode */
SecondaryServer::SecondaryServer(Configuration* c)
: Server(c)
{
}

/* initialization */
void SecondaryServer::init() 
{
	DBG("Starting opening of shared structures...");
	try {
#ifdef __SYSV	
		msgQueue = SharedMessageQueue::open(sysv_shmem,
					      env->getMsgQueueName().c_str());
		segment = SharedMemorySegment::open(sysv_shmem,
						env->getSegmentName().c_str());

#else
		msgQueue = SharedMessageQueue::open(posix_shmem,
						env->getMsgQueueName().c_str());
                       
		segment = SharedMemorySegment::open(posix_shmem,
						env->getSegmentName().c_str());
#endif
	}
	catch(interprocess_exception &ex) {
		ERROR("Error when initializing the server: " << ex.what());
		exit(-1);
	}

	metadataManager = config->getMetadataManager();
	ASSERT(metadataManager != NULL);
	DBG("Metadata manager created successfuly");

	actionsManager = config->getActionsManager();
	ASSERT(actionsManager != NULL);

	DBG("Actions manager created successfuly");

	INFO("Server successfully initialized with configuration " 
			<< config->getFileName());
}

/* destructor */
SecondaryServer::~SecondaryServer()
{
	delete msgQueue;
	delete segment;
		
	Configuration::kill();
}

}
