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
#include <string>
#include <vector>
#include <stdint.h>

#include "Damaris.h"
#include "env/Environment.hpp"
#include "client/StandaloneClient.hpp"
#include "msg/Signals.hpp"
#include "data/ParameterManager.hpp"
#include "action/ActionManager.hpp"
#include "data/VariableManager.hpp"
#include "storage/StorageManager.hpp"
#include "buffer/Mode.hpp"

#ifdef HAVE_VISIT_ENABLED
#include "visit/VisItListener.hpp"
#endif

namespace damaris {


	StandaloneClient::StandaloneClient() : Client()
	{
#ifdef HAVE_VISIT_ENABLED
		if(Environment::GetModel()->visit().present()) {
			VisItListener::Init(Environment::GetEntityComm(),
					Environment::GetModel()->visit(),
					Environment::GetSimulationName());
		}
#endif
	}

	int StandaloneClient::Write(const std::string & varname, 
			int32_t block, const void* data, bool blocking)
	{
		std::shared_ptr<Variable> v = VariableManager::Search(varname);
		if(not v) {
			//ERROR("Undefined variable " << varname);
			return DAMARIS_UNDEFINED_VARIABLE;
		}

		if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
			ERROR("Invalid block id");
			return DAMARIS_INVALID_BLOCK;
		}

		int source = Environment::GetEntityProcessID();
		int iteration = Environment::GetLastIteration();

		std::shared_ptr<Block> b = v->Allocate(source, iteration, block, blocking);

		if(not b) {
			errorOccured_ = 1;
			ERROR("Could not allocated block for variable "<< v->GetName());
			return DAMARIS_ALLOCATION_ERROR;
		}

		DataSpace<Buffer> ds = b->GetDataSpace();

		void* buffer = ds.GetData();
		if(buffer == NULL) {
			ERROR("Invalid dataspace");
			return DAMARIS_DATASPACE_ERROR;
		}

		size_t size = ds.GetSize();
		memcpy(buffer,data,size);

		return DAMARIS_OK;
	}

	int StandaloneClient::Signal(const std::string & signame)
	{
		std::shared_ptr<Action> a = ActionManager::Search(signame);
		if(not a) {
			ERROR("Undefined action " << signame);
			return DAMARIS_UNDEFINED_ACTION;
		}

		int source = Environment::GetEntityProcessID();
		int iteration = Environment::GetLastIteration();

		if(a->GetScope() != model::Scope::bcast)
			(*a)(source,iteration);

		return DAMARIS_OK;
	}

	int StandaloneClient::Alloc(const std::string & varname,
			int32_t block, void** buffer, bool blocking)
	{
		std::shared_ptr<Variable> v = VariableManager::Search(varname);
		if(not v) {
			ERROR("Undefined variable " << varname);
			return DAMARIS_UNDEFINED_VARIABLE;
		}

		if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
			ERROR("Invalid block id");
			return DAMARIS_INVALID_BLOCK;
		}

		int source = Environment::GetEntityProcessID();
		int iteration = Environment::GetLastIteration();

		std::shared_ptr<Block> b = v->Allocate(source, iteration, block, blocking);

		if(not b) {
			ERROR("Could not allocated block for variable "<< v->GetName());
			errorOccured_ = 1;
			return DAMARIS_ALLOCATION_ERROR;
		}

		DataSpace<Buffer> ds = b->GetDataSpace();

		*buffer = ds.GetData();
		if((*buffer) == NULL) {
			ERROR("Invalid dataspace");
			return DAMARIS_DATASPACE_ERROR;
		}

		return DAMARIS_OK;
	}

	int StandaloneClient::Commit(const std::string & varname, 
			int32_t block, int32_t iteration)
	{
		std::shared_ptr<Variable> v = VariableManager::Search(varname);
		if(not v) {
			ERROR("Undefined variable " << varname);
			return DAMARIS_UNDEFINED_VARIABLE;
		}

		if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
			ERROR("Invalid block id");
			return DAMARIS_INVALID_BLOCK;
		}

		int source = Environment::GetEntityProcessID();
		std::shared_ptr<Block> b = v->GetBlock(source,iteration,block);

		if(not b) {
			ERROR("Unable to find corresponding block");
			return DAMARIS_BLOCK_NOT_FOUND;
		}

		b->SetReadOnly(true);

		return DAMARIS_OK;
	}

	int StandaloneClient::Clear(const std::string & varname, 
			int32_t block, int32_t iteration)
	{
		std::shared_ptr<Variable> v = VariableManager::Search(varname);
		if(not v) {
			ERROR("Undefined variable " << varname);
			return DAMARIS_UNDEFINED_VARIABLE;
		}

		if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
			ERROR("Invalid block id");
			return DAMARIS_INVALID_BLOCK;
		}

		int source = Environment::GetEntityProcessID();
		std::shared_ptr<Block> b = v->GetBlock(source,iteration,block);

		if(not b) {
			ERROR("Unable to find corresponding block");
			return DAMARIS_BLOCK_NOT_FOUND;
		}

		b->SetReadOnly(false);
		v->DetachBlock(b);

		return DAMARIS_OK;
	}

	int StandaloneClient::Connect()
	{
		int source = Environment::GetEntityProcessID();
		Environment::AddConnectedClient(source);
		return DAMARIS_OK;
	}

	int StandaloneClient::StopServer()
	{
		return DAMARIS_NO_SERVER;
	}

	int StandaloneClient::EndIteration()
	{
		// do a reduction to know if any process had an error
		int nb_errors;
		MPI_Allreduce(&errorOccured_, &nb_errors,1, 
				MPI_INT, MPI_SUM, Environment::GetEntityComm());

		Environment::StartNextIteration();
		int iteration = Environment::GetLastIteration()-1;

#ifdef HAVE_VISIT_ENABLED
		if(Environment::GetModel()->visit().present()) {
			if(errorOccured_ == 0) {

				int frequency = VisItListener::UpdateFrequency();
				if((frequency > 0) && (iteration % frequency == 0)) {
					VisItListener::Update();
				}

				if(Environment::GetEntityProcessID() == 0) {
				VisItListener::Connected();
			}

			VisItListener::EnterSyncSection(0,0,NULL,0);
		}
	}
#endif
	StorageManager::Update(iteration);

	errorOccured_ = 0;
	return DAMARIS_OK;
}

}
