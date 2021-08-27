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
#include "client/RemoteClient.hpp"
#include "msg/Signals.hpp"
#include "data/ParameterManager.hpp"
#include "action/ActionManager.hpp"
#include "data/VariableManager.hpp"
#include "buffer/Mode.hpp"

namespace damaris {



int RemoteClient::Write(const std::string & varname,
	int32_t block, const void* data, bool UNUSED(blocking))
{

		std::shared_ptr<Variable> v = VariableManager::Search(varname);
		if(not v) {
			//ERROR("Undefined variable " << varname);
			return DAMARIS_UNDEFINED_VARIABLE;
		}

		std::shared_ptr<Layout> l = v->GetLayout();

		if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
			ERROR("Invalid block id");
			return DAMARIS_INVALID_BLOCK;
		}

		int source = Environment::GetEntityProcessID();
		int iteration = Environment::GetLastIteration();
		size_t size  = v->GetLayout()->GetRequiredMemory();

		// sends the remote write header message
		HeaderMessage h;
		h.type_ = DAMARIS_SIG_REMOTE_WRITE;
		h.size_ = sizeof(RemoteWriteMessage);
		int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));

		if(err != DAMARIS_OK) {
			ERROR("Error occurred during AsyncSend"
			<< " of remote write header message");
			return err;
		}
		// sends the remote write message
		RemoteWriteMessage rwm;
		rwm.id_ = v->GetID();
		rwm.block_ = block;
		rwm.source_ = source;
		rwm.iteration_ = iteration;
		rwm.size_   = size;
        
        
        std::vector<int64_t> p ;
        p = v->GetPositions(block) ;

		rwm.dim_ = l->GetDimensions();
		for(int i = 0; i < rwm.dim_; i++) {
			// rwm.lbounds_[i] = 0;
			// rwm.ubounds_[i] = l->GetExtentAlong(i)-1;
            rwm.ubounds_[i] = l->GetExtentAlong(i)-1 + p[i]  ; // Move() has a "- lower_bounds[]" in there too
			rwm.lbounds_[i] = p[i];
			rwm.gbounds_[i] = l->GetGlobalExtentAlong(i);
			rwm.ghosts_[2*i]   = l->GetGhostAlong(i).first;
			rwm.ghosts_[2*i+1] = l->GetGhostAlong(i).second;
		}

		err = ch2server_->AsyncSend(DAMARIS_SIG_BODY,&rwm,sizeof(rwm));

		if(err != DAMARIS_OK) {
			ERROR("Error occurred during AsyncSend"
			<< " of remote write header message-2nd phase");
			return err;
		}
		err = ch2server_->AsyncSend(DAMARIS_SIG_DATA,data,size);
		return err;
}


int RemoteClient::Alloc(const std::string & varname,
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

int RemoteClient::Commit(const std::string & varname,
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

	const DataSpace<Buffer>& ds = b->GetDataSpace();
	size_t size = ds.GetSize();
	void* data = ds.GetData();

	// sends the remote write header message
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_REMOTE_WRITE;
	h.size_ = sizeof(RemoteWriteMessage);
	int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
	
	if(err != DAMARIS_OK) {
		ERROR("Error occurred during AsyncSend of remote write header message");
		return err;
	}

	// sends the remote write message
	RemoteWriteMessage rwm;
	rwm.id_ = v->GetID();
	rwm.block_ = block;
	rwm.source_ = Environment::GetEntityProcessID();
	rwm.iteration_ = Environment::GetLastIteration();
	rwm.size_   = size;
	rwm.dim_ = b->GetDimensions();
	for(int i = 0; i < rwm.dim_; i++) {
		rwm.lbounds_[i] = b->GetStartIndex(i);
		rwm.ubounds_[i] = b->GetEndIndex(i);
		rwm.gbounds_[i] = b->GetGlobalExtent(i);
		rwm.ghosts_[2*i]   = b->GetGhost(i).first;
		rwm.ghosts_[2*i+1] = b->GetGhost(i).second;
	}

	err = ch2server_->AsyncSend(DAMARIS_SIG_BODY,&rwm,sizeof(rwm));

	if(err != DAMARIS_OK) {
		ERROR("Error occurred during AsyncSend of remote write header message-2nd phase");
		return err;
	}

	// sends data
	err = ch2server_->AsyncSend(DAMARIS_SIG_DATA,data,size);
	return err;
}

}
