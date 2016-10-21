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

#include <string>
#include <string.h>
#include <mpi.h>
#include <VisItDataInterface_V2.h>
#include <VisItControlInterface_V2.h>

#include "util/Debug.hpp"
#include "util/Pointers.hpp"
#include "util/Unused.hpp"
#include "data/MeshManager.hpp"
#include "data/CurveManager.hpp"
#include "data/VariableManager.hpp"
#include "action/ActionManager.hpp"
#include "env/Environment.hpp"
#include "visit/VisItListener.hpp"


#define VISIT_COMMAND_PROCESS 0
#define VISIT_COMMAND_SUCCESS 1
#define VISIT_COMMAND_FAILURE 2

namespace damaris {

VisItListener::SimData VisItListener::sim_ = {0};
MPI_Comm VisItListener::comm_ = MPI_COMM_NULL;
int VisItListener::visitstate_ = -1;
unsigned int VisItListener::updatefreq_ = 0;

void VisItListener::Init(MPI_Comm c, 
	const model::Simulation::visit_optional& mdl, 
	const std::string& simname)
{		
	comm_ = c;
	int rank, size;
	MPI_Comm_rank(comm_,&rank);
	MPI_Comm_size(comm_,&size);

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
	
	if(mdl.present()) {
		updatefreq_ = mdl.get().update_frequency();
	}
	
	VisItSetBroadcastIntFunction(&BroadcastIntCallback);
	VisItSetBroadcastStringFunction(&BroadcastStringCallback);
	VisItSetBroadcastIntFunction2(&BroadcastIntCallback2,NULL);
	VisItSetBroadcastStringFunction2(&BroadcastStringCallback2,NULL);

	VisItSetParallel(size > 1);
	VisItSetParallelRank(rank);
	VisItSetMPICommunicator(&comm_);

	char* env = NULL;
	if(rank == 0) {
		env = VisItGetEnvironment();
	}
	VisItSetupEnvironment2(env);
	free(env);

	if(rank == 0) {
		VisItInitializeSocketAndDumpSimFile(simname.c_str(),
				"", "", NULL, NULL, NULL);
	}
}

bool VisItListener::Connected()
{
	// prevents from checking again if it already connected
	// visitstate will be reset after EnterSyncSection
	if(visitstate_ > 0) return false;
	
	visitstate_ = VisItDetectInput(0,-1);
	if(visitstate_ >= -5 && visitstate_ <= -1) {
		ERROR("Uncaught VisIt error");
		return false;
	} else if(visitstate_ == 1) {
		DBG("VisIt first attempt to connect");
	}
	return visitstate_ > 0;
}

void VisItListener::EnterSyncSection(
	int UNUSED(tag), int UNUSED(source), 
	const void* UNUSED(buf), int UNUSED(count))
{
	MPI_Bcast(&visitstate_,1,MPI_INT,0,comm_);
	
	sim_.iteration_ = Environment::GetLastIteration()-1;
	switch(visitstate_) {
	case 1:
		if(VisItAttemptToCompleteConnection() == VISIT_OKAY) {
			VisItSetActivateTimestep(&VisItListener::TimeStepCallback,(void*)(&sim_));
			VisItSetSlaveProcessCallback(&VisItListener::SlaveProcessCallback);	
			VisItSetGetMetaData(&VisItListener::GetMetaData,(void*)(&sim_));
			VisItSetGetMesh(&VisItListener::GetMesh,(void*)(&sim_));
			VisItSetGetCurve(&VisItListener::GetCurve,(void*)(&sim_));
			VisItSetGetVariable(&VisItListener::GetVariable,(void*)(&sim_));
			VisItSetGetDomainList(&VisItListener::GetDomainList,(void*)(&sim_));
			VisItSetCommandCallback(&VisItListener::ControlCommandCallback,(void*)(&sim_));
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
	visitstate_ = 0;
}

void VisItListener::Update()
{
	sim_.iteration_ = Environment::GetLastIteration()-1;
	VisItTimeStepChanged();
	VisItUpdatePlots();
}

unsigned int VisItListener::UpdateFrequency()
{
	return updatefreq_;
}

int VisItListener::TimeStepCallback(void* UNUSED(cbdata))
{
	DBG("Inside TimeStepCallBack");
	return VISIT_OKAY;
}

void VisItListener::BroadcastSlaveCommand(int *command)
{
	MPI_Bcast(command, 1, MPI_INT, 0, comm_);
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
	MPI_Comm_rank(comm_,&rank);

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
	return MPI_Bcast(value, 1, MPI_INT, sender, comm_);
}

int VisItListener::BroadcastIntCallback2(int *value, int sender, 
		void* s __attribute__((unused)))
{
	return MPI_Bcast(value, 1, MPI_INT, sender, comm_);
}

int VisItListener::BroadcastStringCallback(char *str, int len, int sender)
{
	return MPI_Bcast(str, len, MPI_CHAR, sender, comm_);
}

int VisItListener::BroadcastStringCallback2(char *str, int len, int sender, 
		void* UNUSED(s))
{
	return MPI_Bcast(str, len, MPI_CHAR, sender, comm_);
}

void VisItListener::ControlCommandCallback(const char *cmd, const char *args, 
	void *cbdata)
{
	DBG("In VisItListener::ControlCommandCallback");
	SimData* sim = (SimData*)(cbdata);
	shared_ptr<Action> a = ActionManager::Search(std::string(cmd));
	if(a == NULL) {
		ERROR("Damaris received the event \"" << cmd 
			<< "\" which does not correspond to "
			<< "any in configuration");
		return;
	}
	if(not a->IsExternallyVisible()) {
		ERROR("Triggering an action not externally visible: \"" 
			<< cmd << "\"");
		return;
	}
	a->Call(sim->iteration_,-1,args);
}

visit_handle VisItListener::GetMetaData(void *cbdata)
{
	SimData* s = (SimData*)cbdata;
	visit_handle md = VISIT_INVALID_HANDLE;
	DBG("Entering GetMetaData");
	if(VisIt_SimulationMetaData_alloc(&md) == VISIT_OKAY)
	{
		VisIt_SimulationMetaData_setMode(md,VISIT_SIMMODE_RUNNING);
		VisIt_SimulationMetaData_setCycleTime(md, 
			s->iteration_, s->iteration_);
	
		// expose Meshes
		if(not MeshManager::IsEmpty()) {
			MeshManager::iterator mesh = MeshManager::Begin();
			MeshManager::iterator end = MeshManager::End();
			while(mesh != end) {
				(*mesh)->ExposeVisItMetaData(md);
				mesh++;
			}
		}
		DBG("Mesh exposed successfuly");

		// expose Curves
		if(not CurveManager::IsEmpty()) {
			CurveManager::iterator curve = CurveManager::Begin();
			CurveManager::iterator end = CurveManager::End();
			while(curve != end) {
				(*curve)->ExposeVisItMetaData(md);
				curve++;
			}
		}
		DBG("Curve exposed successfuly");

		// expose Variables
		if(not VariableManager::IsEmpty()) { 
			VariableManager::iterator var = VariableManager::Begin();
			VariableManager::iterator end = VariableManager::End();
			while(var != end) {
				(*var)->ExposeVisItMetaData(md,s->iteration_);
				var++;
			}
		}
		DBG("Variables exposed successfuly");

		// expose commands
		if(not ActionManager::IsEmpty()) {
			ActionManager::iterator act = ActionManager::Begin();
			ActionManager::iterator end = ActionManager::End();
			while(act != end) {
				(*act)->ExposeVisItMetaData(md);
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
	shared_ptr<Mesh> m = MeshManager::Search(name);
	visit_handle h = VISIT_INVALID_HANDLE;
	if(m != NULL) {
		DBG("Mesh found, exposing data, iteration is " << s->iteration_);
		
		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();

		int source = domain / nbrLocalBlocksPerClient;
		int block = domain % nbrLocalBlocksPerClient;

		m->ExposeVisItData(&h,source,s->iteration_,block);
	}	
	return h;	
}

visit_handle VisItListener::GetCurve(const char *name, void *cbdata)
{
	SimData *s = (SimData*)cbdata;
	shared_ptr<Curve> c = CurveManager::Search(name);
	visit_handle h = VISIT_INVALID_HANDLE;
	if(c != NULL) {
		c->ExposeVisItData(&h,s->iteration_);
	}
	return h;
}

visit_handle VisItListener::GetVariable(int domain, const char *name, void *cbdata)
{
	DBG("Entering VisItListener::GetVariable for variable " << name);
	SimData *s = (SimData*)cbdata;
	DBG("In GetVariable, iteration is " << s->iteration_ << ", domain is " << domain);
	shared_ptr<Variable> v = VariableManager::Search(name);
	visit_handle h = VISIT_INVALID_HANDLE;
	if(v != NULL) {
		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalBlocksPerClient = Environment::NumDomainsPerClient();

		int source = domain / nbrLocalBlocksPerClient;
		int block = domain % nbrLocalBlocksPerClient;

		v->ExposeVisItData(&h,source,s->iteration_,block);
	} else {
		ERROR("Variable not found: \"" << name << "\"");
	}
	return h;
}

visit_handle VisItListener::GetDomainList(const char* /*name*/, void* /*cbdata*/)
{
	visit_handle h = VISIT_INVALID_HANDLE;
	
	if(VisIt_DomainList_alloc(&h) != VISIT_ERROR)
	{
		visit_handle hdl;
		int *iptr = NULL;

		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalClients = clients.size();
			//Environment::HasServer() ? Environment::ClientsPerNode() : 1;
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
