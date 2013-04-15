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
#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "core/Process.hpp"
#include "core/ProcInfo.hpp"
#include "client/StdAloneClient.hpp"
#include "xml/BcastXML.hpp"
#include "server/Initiator.hpp"

extern Damaris::Server* __server;
extern Damaris::Client* __client;

namespace Damaris {

bool Initiator::mpi_init(const std::string& configFile, MPI_Comm globalcomm)
{
	/* Global rank and size in the passed communicator */
	int size, rank;
	MPI_Comm_size(globalcomm,&size);
	MPI_Comm_rank(globalcomm,&rank);

	std::auto_ptr<Model::Simulation> mdl
		= Model::BcastXML(globalcomm,configFile);

	Process::Init(mdl);
	Process* p = Process::Get();
	/* currently thie Process object had no ID */
	
	Environment::SetGlobalComm(globalcomm);
	int clpn = Environment::ClientsPerNode();
	int copn = Environment::CoresPerNode();

	/* Create a new communicator gathering processes of the same node */
	int color = ProcInfo::GetNodeID();

	MPI_Comm nodecomm;
	MPI_Comm_split(globalcomm,color,rank,&nodecomm);
	Environment::SetNodeComm(nodecomm);
	
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
	Environment::SetEntityComm(entitycomm);
	
	/* Get rank and size in the entity communicator */
	int rankInEnComm, sizeOfEnComm;
	MPI_Comm_rank(entitycomm,&rankInEnComm);
	MPI_Comm_size(entitycomm,&sizeOfEnComm);

	if(not (copn == clpn)) {
		// dedicated core mode : the number of servers to create is strictly positive
		if(is_client) {
			DBG("Client starting, rank = " << rank);
			p->setID(rankInEnComm);
			// the following barrier ensures that the client
			// won't be created before the servers are started.
			MPI_Barrier(globalcomm);
			p->openSharedStructures();
			__client = new Client(p);
			__client->connect();
			Environment::SetClient(true);
		} else {
			DBG("Server starting, rank = " << rank);
			p->createSharedStructures();
			p->setID(rankInEnComm);
			__server = new Server(p);
			Environment::SetClient(false);
			MPI_Barrier(globalcomm);
		}
	} else {
		// synchronous mode : the servers are attached to each client
		DBG("Starting in synchronous mode");
		if(rankInNode != 0) {
			p->setID(rank);
			MPI_Barrier(globalcomm);
			p->openSharedStructures();
			__client = new StdAloneClient(p);
			__client->connect();
			Environment::SetClient(true);
		} else {
			p->setID(rank);
			p->createSharedStructures();
			MPI_Barrier(globalcomm);
			__client = new StdAloneClient(p);
			Environment::SetClient(true);
			__client->connect();
		}
	}
	return true;
}

bool Initiator::start_server()
{
	if(__server != NULL)
	{
		__server->run();
		return true;
	}
	return false;
}

Client* Initiator::mpi_init_and_start(const std::string& configFile, MPI_Comm globalcomm)
{
	mpi_init(configFile,globalcomm);
	start_server();
	return __client;
}

}
