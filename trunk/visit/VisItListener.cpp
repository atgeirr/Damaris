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
#include "visit/VisItListener.hpp"


#define VISIT_COMMAND_PROCESS 0
#define VISIT_COMMAND_SUCCESS 1
#define VISIT_COMMAND_FAILURE 2

namespace Damaris {
namespace Viz {

MPI_Comm VisItListener::comm = MPI_COMM_NULL;

void VisItListener::init(MPI_Comm c, const Model::VisitParam& mdl, const std::string& simname)
{
	if(mdl.path() != "") {
		char* path = (char*)malloc(sizeof(char)*(mdl.path().length()+1));
		strcpy(path,mdl.path().c_str());
		VisItSetDirectory(path);
		free(path);
	}
	VisItSetupEnvironment();
	VisItInitializeSocketAndDumpSimFile(simname.c_str(),"", "", NULL, NULL, NULL);
	comm = c;
	INFO("VisIt-Damaris connection initialized with visit path = " << mdl.path());
}

int VisItListener::connected()
{
	int visitstate = VisItDetectInput(0, -1);
	if(visitstate >= -5 && visitstate <= -1) {
		ERROR("Uncaught VisIt error");
	} else if(visitstate == 1) {
		INFO("VisIt first attempt to connect...");
	}
	return visitstate;
}

int VisItListener::enterSyncSection(int visitstate)
{
	INFO("Entering Sync Section");
	switch(visitstate) {
		case 1:
			if(VisItAttemptToCompleteConnection() == VISIT_OKAY) {
				INFO("VisIt connected");
				
				//VisItSetSlaveProcessCallback(&VisItListener::slaveProcessCallback);
			}
			break;
		case 2:
			if(!processVisItCommand()) {
				VisItDisconnect();
			}
			break;
	}
	return 0;
}

void VisItListener::broadcastSlaveCommand(int *command)
{
	MPI_Bcast(command, 1, MPI_INT, 0, comm);
}


void VisItListener::slaveProcessCallback()
{
	int command = VISIT_COMMAND_PROCESS;
	broadcastSlaveCommand(&command);
}

bool VisItListener::processVisItCommand()
{
	int command;
	int rank;
	MPI_Comm_rank(comm,&rank);

	if(rank == 0) {
		int success = VisItProcessEngineCommand();
		if(success) {
			command = VISIT_COMMAND_SUCCESS;
			broadcastSlaveCommand(&command);
			return true;
		}
		else
		{
			command = VISIT_COMMAND_FAILURE;
			broadcastSlaveCommand(&command);
			return false;
		}
	} else {
		while(true) {
			broadcastSlaveCommand(&command);
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

visit_handle VisItListener::SimGetMetaData(void *cbdata)
{
	SimData* s = (SimData*)cbdata;
	visit_handle md = VISIT_INVALID_HANDLE;
	if(VisIt_SimulationMetaData_alloc(&md) == VISIT_OKAY)
	{
		VisIt_SimulationMetaData_setMode(md,VISIT_SIMMODE_RUNNING);
		VisIt_SimulationMetaData_setCycleTime(md, s->iteration, s->iteration);
		
		{ // expose Meshes
			MeshManager::iterator mesh = MeshManager::Begin();
			MeshManager::iterator end = MeshManager::End();
			while(mesh != end) {
				(*mesh)->exposeVisItMetaData(md);
				mesh++;
			}
		}

		{ // expose Variables
			VariableManager::iterator var = VariableManager::Begin();
			VariableManager::iterator end = VariableManager::End();
			while(var != end) {
				(*var)->exposeVisItMetaData(md);
			}
		}
	}
	return md;
}

visit_handle VisItListener::SimGetMesh(int domain, const char *name, void *cbdata)
{
	SimData *s = (SimData*)cbdata;
	Mesh* m = MeshManager::Search(std::string(name));
	visit_handle h = VISIT_INVALID_HANDLE;
	if(m != NULL) {
		m->exposeVisItData(&h,domain,s->iteration);
	}	
	return h;	
}

visit_handle VisItListener::SimGetVariable(int domain, const char *name, void *cbdata)
{
	SimData *s = (SimData*)cbdata;
	Variable* v = VariableManager::Search(std::string(name));
	visit_handle h = VISIT_INVALID_HANDLE;
	if(v != NULL) {
		v->exposeVisItData(&h,domain,s->iteration);
	}
	return h;
}

}
}
