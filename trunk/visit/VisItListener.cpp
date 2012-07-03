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
	if(mdl.present()) {
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
	
	VisItSetupEnvironment();
	comm = c;

	VisItSetBroadcastIntFunction(&BroadcastIntCallback);
	VisItSetBroadcastStringFunction(&BroadcastStringCallback);

	int rank, size;
	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&size);

	VisItSetParallel(size > 1);
	VisItSetParallelRank(rank);
	VisItSetMPICommunicator(&comm);
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
		INFO("VisIt first attempt to connect");
	}
	return visitstate;
}

int VisItListener::EnterSyncSection(int visitstate)
{
	DBG("Entering Sync Section, visit state is " << visitstate);
	sim.iteration = Environment::GetLastIteration();
	switch(visitstate) {
		case 1:
			if(VisItAttemptToCompleteConnection() == VISIT_OKAY) {
				INFO("VisIt connected");
				VisItSetSlaveProcessCallback(&VisItListener::SlaveProcessCallback);	
				VisItSetGetMetaData(&VisItListener::GetMetaData,(void*)(&sim));
				VisItSetGetMesh(&VisItListener::GetMesh,(void*)(&sim));
				VisItSetGetVariable(&VisItListener::GetVariable,(void*)(&sim));
				VisItSetGetDomainList(&VisItListener::GetDomainList,(void*)(&sim));
				VisItSetCommandCallback(&VisItListener::ControlCommandCallback,(void*)(&sim));
			} else {
				INFO("VisIt did not connect");
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
	sim.iteration = Environment::GetLastIteration();
	VisItTimeStepChanged();
	VisItUpdatePlots();
	return 0;
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

int VisItListener::BroadcastStringCallback(char *str, int len, int sender)
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
	a->call(sim->iteration,-1,args);
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
				(*mesh)->exposeVisItMetaData(md);
				mesh++;
			}
		}
		DBG("Mesh exposed successfuly");

		// expose Variables
		if(not VariableManager::IsEmpty()) { 
			VariableManager::iterator var = VariableManager::Begin();
			VariableManager::iterator end = VariableManager::End();
			while(var != end) {
				(*var)->exposeVisItMetaData(md);
				var++;
			}
		}
		DBG("Variables exposed successfuly");

		// expose commands
		if(not ActionManager::IsEmpty()) {
			ActionManager::iterator act = ActionManager::Begin();
			ActionManager::iterator end = ActionManager::End();
			while(act != end) {
				(*act)->exposeVisItMetaData(md);
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
		m->exposeVisItData(&h,domain,s->iteration);
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
		v->exposeVisItData(&h,domain,s->iteration);
	} else {
		ERROR("Variable not found: \"" << name << "\"");
	}
	return h;
}

visit_handle VisItListener::GetDomainList(const char* name, void* cbdata)
{
	visit_handle h = VISIT_INVALID_HANDLE;
	if(VisIt_DomainList_alloc(&h) != VISIT_ERROR)
	{
		visit_handle hdl;
		int *iptr = NULL;

		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrClients = clients.size();
		int ttlClients = Environment::GetGlobalNumberOfClients();

		DBG("nbrClients = " << nbrClients << " ttlClients = " << ttlClients);

		std::list<int>::const_iterator it = clients.begin();
		iptr = (int *)malloc(sizeof(int)*nbrClients);
		for(int i = 0; i < nbrClients; i++) {
			iptr[i] = *it;
			it++;
		}

		if(VisIt_VariableData_alloc(&hdl) == VISIT_OKAY)
		{
			VisIt_VariableData_setDataI(hdl, VISIT_OWNER_VISIT, 1, nbrClients, iptr);
			VisIt_DomainList_setDomains(h, ttlClients, hdl);
		} else {
			free(iptr);
		}
	}
	return h;
}

}
}
