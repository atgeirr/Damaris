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
 * \file VisItListener.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include <string>
#include <string.h>
#include <mpi.h>
#include <VisItDataInterface_V2.h>
#include <VisItControlInterface_V2.h>
#include "core/Debug.hpp"
#include "core/MeshManager.hpp"
#include "core/VariableManager.hpp"
#include "core/ActionManager.hpp"
#include "core/Environment.hpp"
#include "visit/VisItListener.hpp"


#define VISIT_COMMAND_PROCESS 0
#define VISIT_COMMAND_SUCCESS 1
#define VISIT_COMMAND_FAILURE 2

namespace Damaris {
namespace Viz {

VisItListener::SimData VisItListener::sim = {0};
MPI_Comm VisItListener::comm = MPI_COMM_NULL;

void VisItListener::Init(MPI_Comm c, const Model::Simulation::visit_optional& mdl, 
				const std::string& simname)
{
	comm = c;
	int rank, size;
	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&size);

	if(mdl.present() && (rank == 0)) {
		char* s;
		if(mdl.get().path().present()) {
			s = (char*)malloc(mdl.get().path().get().length()+1);
			strcpy(s,mdl.get().path().get().c_str());
			VisItSetDirectory(s);
			free(s);
		}
		if(mdl.get().options().present()) {
			s = (char*)malloc(mdl.get().options().get().length()+1);
			strcpy(s,mdl.get().options().get().c_str());
			VisItSetOptions(s);
			free(s);
		}
	}
	
	VisItSetBroadcastIntFunction(&BroadcastIntCallback);
	VisItSetBroadcastStringFunction(&BroadcastStringCallback);
	VisItSetBroadcastIntFunction2(&BroadcastIntCallback2,NULL);
	VisItSetBroadcastStringFunction2(&BroadcastStringCallback2,NULL);

	VisItSetParallel(size > 1);
	VisItSetParallelRank(rank);
	VisItSetMPICommunicator(&comm);

	char* env = NULL;
	if(rank == 0) {
		env = VisItGetEnvironment();
	}
	VisItSetupEnvironment2(env);
	free(env);

	if(rank == 0) {
		VisItInitializeSocketAndDumpSimFile(simname.c_str(),"", "", NULL, NULL, NULL);
	}
}

int VisItListener::Connected()
{
	//int visitstate = VisItDetectInputWithTimeout(0,1000,-1);
	int visitstate = VisItDetectInput(0,-1);
	if(visitstate >= -5 && visitstate <= -1) {
		ERROR("Uncaught VisIt error");
	} else if(visitstate == 1) {
		DBG("VisIt first attempt to connect");
	}
	return visitstate;
}

int VisItListener::EnterSyncSection(int visitstate)
{
	DBG("Entering Sync Section, visit state is " << visitstate);
	sim.iteration = Environment::GetLastIteration()-1;
	switch(visitstate) {
		case 1:
			if(VisItAttemptToCompleteConnection() == VISIT_OKAY) {
				DBG("VisIt connected");
				VisItSetActivateTimestep(&VisItListener::TimeStepCallback,(void*)(&sim));
				VisItSetSlaveProcessCallback(&VisItListener::SlaveProcessCallback);	
				VisItSetGetMetaData(&VisItListener::GetMetaData,(void*)(&sim));
				VisItSetGetMesh(&VisItListener::GetMesh,(void*)(&sim));
				VisItSetGetVariable(&VisItListener::GetVariable,(void*)(&sim));
				VisItSetGetDomainList(&VisItListener::GetDomainList,(void*)(&sim));
				VisItSetCommandCallback(&VisItListener::ControlCommandCallback,(void*)(&sim));
			} else {
				ERROR("VisIt did not connect");
			}
			break;
		case 2:
			if(!ProcessVisItCommand()) {
				VisItDisconnect();
			}
			break;
	}
	return 0;
}

int VisItListener::Update()
{
	sim.iteration = Environment::GetLastIteration()-1;
	VisItTimeStepChanged();
	VisItUpdatePlots();
	return 0;
}

int VisItListener::TimeStepCallback(void* cbdata)
{
	DBG("Inside TimeStepCallBack");
	//VariableManager::ForEach
	return VISIT_OKAY;
}

void VisItListener::BroadcastSlaveCommand(int *command)
{
	MPI_Bcast(command, 1, MPI_INT, 0, comm);
}


void VisItListener::SlaveProcessCallback()
{
	int command = VISIT_COMMAND_PROCESS;
	BroadcastSlaveCommand(&command);
}

bool VisItListener::ProcessVisItCommand()
{
	int command;
	int rank;
	MPI_Comm_rank(comm,&rank);

	if(rank == 0) {
		int success = VisItProcessEngineCommand();
		if(success) {
			command = VISIT_COMMAND_SUCCESS;
			BroadcastSlaveCommand(&command);
			return true;
		}
		else
		{
			command = VISIT_COMMAND_FAILURE;
			BroadcastSlaveCommand(&command);
			return false;
		}
	} else {
		while(true) {
			BroadcastSlaveCommand(&command);
			switch (command)
			{
				case VISIT_COMMAND_PROCESS:
					VisItProcessEngineCommand();
					break;
				case VISIT_COMMAND_SUCCESS:
					return true;
				case VISIT_COMMAND_FAILURE:
					return false;
			}
		}
	}
	return true;
}

int VisItListener::BroadcastIntCallback(int *value, int sender)
{
	return MPI_Bcast(value, 1, MPI_INT, sender, comm);
}

int VisItListener::BroadcastIntCallback2(int *value, int sender, void* s)
{
	return MPI_Bcast(value, 1, MPI_INT, sender, comm);
}

int VisItListener::BroadcastStringCallback(char *str, int len, int sender)
{
	return MPI_Bcast(str, len, MPI_CHAR, sender, comm);
}

int VisItListener::BroadcastStringCallback2(char *str, int len, int sender, void* s)
{
	return MPI_Bcast(str, len, MPI_CHAR, sender, comm);
}

void VisItListener::ControlCommandCallback(const char *cmd, const char *args, void *cbdata)
{
	DBG("In VisItListener::ControlCommandCallback");
	SimData* sim = (SimData*)(cbdata);
	Action* a = ActionManager::Search(std::string(cmd));
	if(a == NULL) {
		ERROR("Damaris received the event \"" << cmd << "\" which does not correspond to "
			<< "any in configuration");
		return;
	}
	if(not a->IsExternallyVisible()) {
		ERROR("Triggering an action not externally visible: \"" << cmd << "\"");
		return;
	}
	a->Call(sim->iteration,-1,args);
}

visit_handle VisItListener::GetMetaData(void *cbdata)
{
	SimData* s = (SimData*)cbdata;
	visit_handle md = VISIT_INVALID_HANDLE;
	DBG("Entering GetMetaData");
	if(VisIt_SimulationMetaData_alloc(&md) == VISIT_OKAY)
	{
		VisIt_SimulationMetaData_setMode(md,VISIT_SIMMODE_RUNNING);
		VisIt_SimulationMetaData_setCycleTime(md, s->iteration, s->iteration);
	
		// expose Meshes
		if(not MeshManager::IsEmpty()) {
			MeshManager::iterator mesh = MeshManager::Begin();
			MeshManager::iterator end = MeshManager::End();
			while(mesh != end) {
				(*mesh)->ExposeVisItMetaData(md,s->iteration);
				mesh++;
			}
		}
		DBG("Mesh exposed successfuly");

		// expose Variables
		if(not VariableManager::IsEmpty()) { 
			VariableManager::iterator var = VariableManager::Begin();
			VariableManager::iterator end = VariableManager::End();
			while(var != end) {
				(*var)->ExposeVisItMetaData(md,s->iteration);
				var++;
			}
		}
		DBG("Variables exposed successfuly");

		// expose commands
		if(not ActionManager::IsEmpty()) {
			ActionManager::iterator act = ActionManager::Begin();
			ActionManager::iterator end = ActionManager::End();
			while(act != end) {
				(*act)->ExposeVisItMetaData(md,s->iteration);
				act++;
			}
		}
		DBG("Actions exposed successfuly");
	}
	return md;
}

visit_handle VisItListener::GetMesh(int domain, const char *name, void *cbdata)
{
	DBG("Entering VisItListener::GetMesh for mesh " << name << ", domain is " << domain);
	SimData *s = (SimData*)cbdata;
	Mesh* m = MeshManager::Search(std::string(name));
	visit_handle h = VISIT_INVALID_HANDLE;
	if(m != NULL) {
		DBG("Mesh found, exposing data, iteration is " << s->iteration);
		
		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();

		int source = domain / nbrLocalBlocksPerClient;
		int block = domain % nbrLocalBlocksPerClient;

		m->ExposeVisItData(&h,source,s->iteration,block);
	}	
	return h;	
}

visit_handle VisItListener::GetVariable(int domain, const char *name, void *cbdata)
{
	DBG("Entering VisItListener::GetVariable for variable " << name);
	SimData *s = (SimData*)cbdata;
	DBG("In GetVariable, iteration is " << s->iteration << ", domain is " << domain);
	Variable* v = VariableManager::Search(std::string(name));
	visit_handle h = VISIT_INVALID_HANDLE;
	if(v != NULL) {
		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();

		int source = domain / nbrLocalBlocksPerClient;
		int block = domain % nbrLocalBlocksPerClient;

		v->ExposeVisItData(&h,source,s->iteration,block);
	} else {
		ERROR("Variable not found: \"" << name << "\"");
	}
	return h;
}

visit_handle VisItListener::GetDomainList(const char* name, void* cbdata)
{
	/* This is the code that should work when VisIt will handle
	 * one domain list per variable... 
	Variable* var = VariableManager::Search(std::string(name));
	if(var == NULL) {
		ERROR("VisIt requested domain list for an unknown variable: " << name);
		return VISIT_INVALID_HANDLE;
	}
	
	SimData *s = (SimData*)cbdata;
	visit_handle h = VISIT_INVALID_HANDLE;

	if(var->ExposeVisItDomainList(&h,s->iteration))
		return h;
	else
		return VISIT_INVALID_HANDLE;
	*/

	visit_handle h = VISIT_INVALID_HANDLE;
	
	if(VisIt_DomainList_alloc(&h) != VISIT_ERROR)
	{
		visit_handle hdl;
		int *iptr = NULL;

		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalClients = Environment::HasServer() ? Environment::ClientsPerNode() : 1;
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();
		int nbrLocalBlocks = nbrLocalClients*nbrLocalBlocksPerClient;
		int ttlClients = Environment::CountTotalClients();
		int ttlBlocks = ttlClients*nbrLocalBlocksPerClient;

		std::list<int>::const_iterator it = clients.begin();
		iptr = (int *)malloc(sizeof(int)*nbrLocalBlocks);
		for(int i = 0; i < nbrLocalClients; i++) {
			for(int j = 0; j < nbrLocalBlocksPerClient; j++) {
				iptr[i*nbrLocalBlocksPerClient+j] 
					= (*it)*nbrLocalBlocksPerClient + j;
			}
			it++;
		}

		if(VisIt_VariableData_alloc(&hdl) == VISIT_OKAY)
		{
			VisIt_VariableData_setDataI(hdl, VISIT_OWNER_VISIT, 1,
					nbrLocalBlocks, iptr);
			DBG(nbrLocalBlocks);
			VisIt_DomainList_setDomains(h, ttlBlocks, hdl);
			DBG(ttlBlocks);
		} else {
			free(iptr);
			return VISIT_INVALID_HANDLE;
		}
	}
	return h;
}
}
}
