/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/
#include "Damaris.h"
#include "env/Environment.hpp"
#include "data/ParameterManager.hpp"
#include "action/BoundAction.hpp"
#include "action/ActionManager.hpp"

using namespace damaris;

extern "C" {

int damaris_initialize(const char* configfile, MPI_Comm comm)
{
    Environment::Log("damaris_initialize() started" , EventLogger::Info);

	if(Environment::Initialized()) {
		return DAMARIS_ALREADY_INITIALIZED;
	}
	
	if(Environment::Init(std::string(configfile),comm)) {
		return DAMARIS_OK;
	} else {
		return DAMARIS_INIT_ERROR;
	}
}

int damaris_finalize()
{
    Environment::Log("damaris_finalize() started." , EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	
	if(Environment::Finalize()) {
		return DAMARIS_OK;
	} else {
		return DAMARIS_FINALIZE_ERROR;
	}
}

int damaris_start(int* is_client)
{
    Environment::Log("damaris_start method started." , EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	
	MPI_Barrier(Environment::GetGlobalComm());
	
	if(Environment::HasServer()) {
		if(Environment::IsServer()) {
			*is_client = 0;
			Environment::GetServer()->Run();
		} else {
			*is_client = 1;
		}
	} else {
		*is_client = 1;
		return DAMARIS_NO_SERVER;
	}
	return DAMARIS_OK;
}

int damaris_stop()
{
	Environment::Log("damaris_stop method started." , EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	
	if(Environment::HasServer() && Environment::IsClient()) {
		int err = Environment::GetClient()->StopServer();
		return err;
	} else {
		return DAMARIS_NO_SERVER;
	}
}

int damaris_write(const char* varname, const void* data)
{
	Environment::Log("damaris_write method started.", EventLogger::Info);

	damaris_write_block(varname,0,data);
	return DAMARIS_OK;
}

int damaris_write_block(const char* varname, int32_t block, const void* data)
{
	Environment::Log("damaris_write_block method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	
	if(not Environment::IsClient()) {
		return DAMARIS_CORE_IS_SERVER;
	}
	
	return Environment::GetClient()->Write(varname,block,data);
}

int damaris_alloc(const char* varname, void** ptr)
{
	Environment::Log("damaris_alloc method started.", EventLogger::Info);

	return damaris_alloc_block(varname,0,ptr);
}

int damaris_alloc_block(const char* varname, int32_t block, void** ptr)
{
	Environment::Log("damaris_alloc_block method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	return Environment::GetClient()->Alloc(varname,block,ptr);
}

int damaris_commit(const char* varname)
{
	Environment::Log("damaris_commit method started.", EventLogger::Info);

	int iteration = Environment::GetLastIteration();
	return damaris_commit_block_iteration(varname,0,iteration);
}

int damaris_commit_block(const char* varname, int32_t block)
{
	Environment::Log("damaris_commit_block method started.", EventLogger::Info);

	int iteration = Environment::GetLastIteration();
	return damaris_commit_block_iteration(varname,block,iteration);
}

int damaris_commit_iteration(const char* varname, int32_t iteration)
{
	Environment::Log("damaris_commit_iteration method started.", EventLogger::Info);

	return damaris_commit_block_iteration(varname,0,iteration);
}

int damaris_commit_block_iteration(const char* varname, 
	int32_t block, int32_t iteration)
{
	Environment::Log("damaris_commit_block method II started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	return Environment::GetClient()->Commit(varname,block,iteration);
}

int damaris_clear(const char* varname)
{
	Environment::Log("damaris_clear method started.", EventLogger::Info);

	int iteration = Environment::GetLastIteration();
	return damaris_clear_block_iteration(varname,0,iteration);
}

int damaris_clear_block(const char* varname, int32_t block)
{
	Environment::Log("damaris_clear_block method started.", EventLogger::Info);

	int iteration = Environment::GetLastIteration();
	return damaris_clear_block_iteration(varname,block,iteration);
}

int damaris_clear_iteration(const char* varname, int32_t iteration)
{
	Environment::Log("damaris_clear_iteartion method II started.", EventLogger::Info);

	return damaris_clear_block_iteration(varname,0,iteration);
}

int damaris_clear_block_iteration(const char* varname, int32_t block, 
	int32_t iteration)
{
	Environment::Log("damaris_clear_block_iteration method II started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	return Environment::GetClient()->Clear(varname,block,iteration);
}


int damaris_signal(const char* signal_name)
{
	Environment::Log("damaris_signal method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	
	return Environment::GetClient()->Signal(std::string(signal_name));
}

int damaris_bind(const char* signal_name, signal_t sig)
{
	Environment::Log("damaris_bind method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	// TODO ensure this is called by ALL the processes
	shared_ptr<Action> a = BoundAction::New<Action>(sig,signal_name);
	if(not a) return DAMARIS_BIND_ERROR;
	
	ActionManager::Add(a);
	return DAMARIS_OK;
}


int damaris_parameter_get(const char* name, 
	void* buffer, unsigned int size)
{
	Environment::Log("damaris_parameter_get method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	shared_ptr<Parameter> p = ParameterManager::Search(name);
	if(p) return p->ToBuffer(buffer, size);
	else return DAMARIS_UNDEFINED_PARAMETER;
}

int damaris_parameter_set(const char* name, 
	const void* buffer, unsigned int size)
{
	Environment::Log("damaris_parameter_set method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	shared_ptr<Parameter> p = ParameterManager::Search(name);
	if(p) {
		return p->FromBuffer(buffer, size);
	} else {
		return DAMARIS_UNDEFINED_PARAMETER;
	}
}

int damaris_set_position(const char* var_name, const int64_t* position)
{
	Environment::Log("damaris_set_position method started.", EventLogger::Info);

	return damaris_set_block_position(var_name,0,position);
}

int damaris_set_block_position(const char* var_name, int32_t block,
	const int64_t* position)
{
	Environment::Log("damaris_set_block_position method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	return Environment::GetClient()->SetPosition(var_name,block,position);
}


int damaris_client_comm_get(MPI_Comm* comm)
{
	Environment::Log("damaris_client_comm_get method started.", EventLogger::Info);

	if(Environment::Initialized()) {
		*comm = Environment::GetEntityComm();
		return DAMARIS_OK;
	} else {
		return DAMARIS_NOT_INITIALIZED;
	}
}

int damaris_end_iteration()
{
	Environment::Log("damaris_end_iteration method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	Environment::GetClient()->EndIteration();
	return DAMARIS_OK;
}

int damaris_get_iteration(int* iteration)
{
	Environment::Log("damaris_get_iteration method started.", EventLogger::Info);

	if(not Environment::Initialized()) {
		return DAMARIS_NOT_INITIALIZED;
	}
	*iteration = Environment::GetLastIteration();
	return DAMARIS_OK;
}

}
