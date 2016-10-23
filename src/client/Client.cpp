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
#include "client/Client.hpp"
#include "msg/Signals.hpp"
#include "data/ParameterManager.hpp"
#include "action/ActionManager.hpp"
#include "data/VariableManager.hpp"
#include "buffer/Mode.hpp"

namespace damaris {

USING_POINTERS;


int Client::Write(const std::string & varname, 
	int32_t block, const void* data, bool blocking)
{
	shared_ptr<Variable> v = VariableManager::Search(varname);
	if(not v) return DAMARIS_UNDEFINED_VARIABLE;
	
	if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
		return DAMARIS_INVALID_BLOCK;
	}
	
	int source = Environment::GetEntityProcessID();
	int iteration = Environment::GetLastIteration();
	
	shared_ptr<Block> b = v->Allocate(source, iteration, block, blocking);
	
	if(not b) {
		errorOccured_ = 1;
		return DAMARIS_ALLOCATION_ERROR;
	}
	
	v->DetachBlock(b);
	DataSpace<Buffer> ds = b->GetDataSpace();
	b->LoseDataOwnership();
	
	void* buffer = ds.GetData();
	if(buffer == NULL) {
		ERROR("DataSpace error");
		return DAMARIS_DATASPACE_ERROR;
	}
	
	size_t size = ds.GetSize();
	memcpy(buffer,data,size);
	
	// sends the header message
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_WRITE;
	h.size_ = sizeof(WriteMessage);
	int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
	
	if(err != DAMARIS_OK) {
		ERROR("Error occured during AsyncSend");
		return err;
	}
	
	// sends the write message
	WriteMessage wm;
	wm.id_ = v->GetID();
	wm.block_ = block;
	wm.source_ = Environment::GetEntityProcessID();
	wm.iteration_ = Environment::GetLastIteration();
	wm.handle_ = v->GetBuffer()->GetHandleFromAddress(buffer);
	
	wm.dim_ = b->GetDimensions();
	for(int i = 0; i < wm.dim_; i++) {
		wm.lbounds_[i] = b->GetStartIndex(i);
		wm.ubounds_[i] = b->GetEndIndex(i);
		wm.gbounds_[i] = b->GetGlobalExtent(i);
		wm.ghosts_[2*i]   = b->GetGhost(i).first;
		wm.ghosts_[2*i+1] = b->GetGhost(i).second;
	}
	
	err = ch2server_->AsyncSend(DAMARIS_SIG_BODY,&wm,sizeof(wm));
	
	return err;
}

int Client::Signal(const std::string & signame)
{
	shared_ptr<Action> a = ActionManager::Search(signame);
	if(not a) {
		return DAMARIS_UNDEFINED_ACTION;
	}

	if(a->GetExecLocation() == model::Exec::local) {
		a->Call(Environment::GetEntityProcessID(),
			Environment::GetLastIteration());
		return DAMARIS_OK;
	}
	
	HeaderMessage h;
	if(a->GetScope() == model::Scope::bcast) {
		h.type_ = DAMARIS_SIG_BCAST_ACTION;
	} else { 
		h.type_ = DAMARIS_SIG_CALL_ACTION;
	}
	h.size_ = sizeof(EventMessage);
	int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
	
	if(err == DAMARIS_OK) {
		EventMessage sig;
		sig.id_ = a->GetID();
		sig.source_ = Environment::GetEntityProcessID();
		sig.iteration_ = Environment::GetLastIteration();
	
		return ch2server_->AsyncSend(DAMARIS_SIG_BODY,
					&sig,sizeof(sig));
	} else {
		return err;
	}
}

int Client::Alloc(const std::string & varname,
	int32_t block, void** buffer, bool blocking)
{
	shared_ptr<Variable> v = VariableManager::Search(varname);
	if(not v) return DAMARIS_UNDEFINED_VARIABLE;
	
	if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
		return DAMARIS_INVALID_BLOCK;
	}
	
	int source = Environment::GetEntityProcessID();
	int iteration = Environment::GetLastIteration();
	
	shared_ptr<Block> b = v->AllocateAligned(source, 
					iteration, block, blocking);
	
	if(not b) {
		errorOccured_ = 1;
		return DAMARIS_ALLOCATION_ERROR;
	}
	
	DataSpace<Buffer> ds = b->GetDataSpace();
	
	*buffer = ds.GetData();
	if((*buffer) == NULL) return DAMARIS_DATASPACE_ERROR;
	
	return DAMARIS_OK;
}

int Client::Commit(const std::string & varname, 
	int32_t block, int32_t iteration)
{
	shared_ptr<Variable> v = VariableManager::Search(varname);
	if(not v) return DAMARIS_UNDEFINED_VARIABLE;
	
	if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
		return DAMARIS_INVALID_BLOCK;
	}
	
	int source = Environment::GetEntityProcessID();
	shared_ptr<Block> b = v->GetBlock(source,iteration,block);
	
	if(not b) return DAMARIS_BLOCK_NOT_FOUND;
	
	b->SetReadOnly(true);
	b->LoseDataOwnership();
	
	// sends the header message
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_COMMIT;
	h.size_ = sizeof(CommitMessage);
	int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
	
	if(err != DAMARIS_OK) {
		return err;
	}
	
	// sends the write message
	CommitMessage m;
	m.id_ = v->GetID();
	m.block_ = block;
	m.source_ = Environment::GetEntityProcessID();
	m.iteration_ = Environment::GetLastIteration();
	
	void* addr = b->GetDataSpace().GetData();
	m.handle_ = v->GetBuffer()->GetHandleFromAddress(addr);
	
	m.dim_ = b->GetDimensions();
	for(int i = 0; i < m.dim_; i++) {
		m.lbounds_[i] = b->GetStartIndex(i);
		m.ubounds_[i] = b->GetEndIndex(i);
		m.gbounds_[i] = b->GetGlobalExtent(i);
		m.ghosts_[2*i]   = b->GetGhost(i).first;
		m.ghosts_[2*i+1] = b->GetGhost(i).second;
	}
	
	err = ch2server_->AsyncSend(DAMARIS_SIG_BODY,&m,sizeof(m));
	
	return err;
}

int Client::Clear(const std::string & varname, 
	int32_t block, int32_t iteration)
{
	shared_ptr<Variable> v = VariableManager::Search(varname);
	if(not v) return DAMARIS_UNDEFINED_VARIABLE;
	
	if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
		return DAMARIS_INVALID_BLOCK;
	}
	
	int source = Environment::GetEntityProcessID();
	shared_ptr<Block> b = v->GetBlock(source,iteration,block);
	
	if(not b) return DAMARIS_BLOCK_NOT_FOUND;
	b->SetReadOnly(false);
	v->DetachBlock(b);
	
	// sends the header message
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_CLEAR;
	h.size_ = sizeof(ClearMessage);
	int err = ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
	
	if(err != DAMARIS_OK) {
		return err;
	}
	
	// sends the write message
	ClearMessage m;
	m.id_ = v->GetID();
	m.block_ = block;
	m.source_ = Environment::GetEntityProcessID();
	m.iteration_ = Environment::GetLastIteration();
	
	err = ch2server_->AsyncSend(DAMARIS_SIG_BODY,&m,sizeof(m));
	
	return err;
}

int Client::SetPosition(const std::string& varname, int32_t block,
	const int64_t* position)
{
	shared_ptr<Variable> v = VariableManager::Search(varname);
	if(not v) return DAMARIS_UNDEFINED_VARIABLE;
	
	if(block < 0 || block >= (int32_t)Environment::NumDomainsPerClient()) {
		return DAMARIS_INVALID_BLOCK;
	}
	
	int d = v->GetLayout()->GetDimensions();
	std::vector<int64_t> p(position, position+d);
	
	return v->SetPosition(block,p);
}

int Client::StopServer()
{
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_STOP;
	h.size_ = 0;
	return ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
}

int Client::Connect()
{
	HeaderMessage h;
	h.type_ = DAMARIS_SIG_CONNECT;
	h.size_ = 0;
	return ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
}

int Client::EndIteration()
{
	HeaderMessage h;
	h.size_ = 0;
	// do a reduction to know if any process had an error
	int nb_errors;
	MPI_Allreduce(&errorOccured_, &nb_errors,1, 
		MPI_INT, MPI_SUM, Environment::GetEntityComm());
	Environment::StartNextIteration();
	
	if(nb_errors == 0) h.type_ = DAMARIS_SIG_NEXT_ITERATION;
	else h.type_ = DAMARIS_SIG_NEXT_ITERATION_ERR;
	
	errorOccured_ = 0;
	
	return ch2server_->AsyncSend(DAMARIS_SIG_HEADER,&h,sizeof(h));
}

}
