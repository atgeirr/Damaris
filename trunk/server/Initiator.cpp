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
 * \file Initiator.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <iostream>
#include <list>
#include "common/Debug.hpp"
#include "common/Environment.hpp"
#include "common/Process.hpp"
#include "client/StdAloneClient.hpp"
#include "server/Initiator.hpp"

namespace Damaris {

Client* Initiator::start(const std::string& configFile, MPI_Comm globalcomm)
{
	/* Global rank and size in the passed communicator */
	int size, rank;
	MPI_Comm_size(globalcomm,&size);
	MPI_Comm_rank(globalcomm,&rank);

	Process::initialize(configFile,rank);
	Process* p = Process::get();
	
	Environment* env = p->getEnvironment();
	env->setGlobalComm(globalcomm);
	int clpn = env->getClientsPerNode();
	int copn = env->getCoresPerNode();

	/* The name of the processor is used to compute communicators */
	char procname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Get_processor_name(procname,&len);

	/* Compute the node identifier from the name */
	uint64_t nhash = (uint64_t)(14695981039346656037ULL);
	uint64_t fnv =  ((uint64_t)1 << 40) + (1 << 8) + 0xb3;
	for(int i=0; i < len; i++) {
		uint64_t c = (uint64_t)(procname[i]);
		nhash = nhash xor c;
		nhash *= fnv;
	}

	/* Create a new communicator gathering processes of the same node */
	int color = ((int)nhash >= 0) ? (int)nhash : - ((int)nhash);
	MPI_Comm nodecomm;
	MPI_Comm_split(globalcomm,color,rank,&nodecomm);
	env->setNodeComm(nodecomm);
	
	/* Get the size and rank in the node */
	int rankInNode;
	int sizeOfNode;
	MPI_Comm_rank(nodecomm,&rankInNode);
	MPI_Comm_size(nodecomm,&sizeOfNode);
	
	if(copn - clpn > 1) {
		ERROR("The number of dedicated cores per node"
			<< " must be either 0 or 1 in this version. Aborting...");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Check that the values match */
	if(sizeOfNode != copn) {
		ERROR("The number of cores detected in node does not match the number" 
			<< " provided in configuration."
			<< " This may be due to a configuration error or a (unprobable)"
			<< " hash colision in the algorithm. Aborting...");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Compute the communcator for clients and servers */
	int is_client = (rankInNode >= clpn) ? 0 : 1;
	MPI_Comm entitycomm;
	MPI_Comm_split(globalcomm,is_client,rank,&entitycomm);
	env->setEntityComm(entitycomm);
	
	/* Get rank and size in the entity communicator */
	int rankInEnComm, sizeOfEnComm;
	MPI_Comm_rank(entitycomm,&rankInEnComm);
	MPI_Comm_size(entitycomm,&sizeOfEnComm);

	if(not (copn == clpn)) {
		// dedicated core mode : the number of servers to create is strictly positive
		if(is_client) {
			DBG("Client starting, rank = " << rank);
			// the following barrier ensures that the client
			// won't be created before the servers are started.
			MPI_Barrier(globalcomm);
			p->openSharedStructures();
			return new Client(p);
		} else {
			DBG("Server starting, rank = " << rank);
			p->createSharedStructures();
			Server server(p);
			MPI_Barrier(globalcomm);
			server.run();
			DBG("After Server.run");
			return NULL;
		}
	} else {
		// synchronous mode : the servers are attached to each client
		if(rankInNode != 0) {
			MPI_Barrier(globalcomm);
			p->openSharedStructures();
			return new StdAloneClient(p);
		} else {
			p->createSharedStructures();
			Client* c = new StdAloneClient(p);
			MPI_Barrier(globalcomm);
			return c;
		}
		return NULL;
	}
}	

}
