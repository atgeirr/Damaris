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

#include <iterator>

#include "util/Unused.hpp"
#include "env/Environment.hpp"
#include "data/Variable.hpp"
#include "data/Type.hpp"

namespace damaris {

bool Variable::AttachBlock(const shared_ptr<Block>& block)
{
	if(not block) return false;
	if(not IsTimeVarying()) {
		block->iteration_ = 0;
	}
	return blocks_.insert(block).second;
}

shared_ptr<Block> Variable::GetBlock(int source, int iteration, int block) const
{
	if(not IsTimeVarying()) {
		iteration = 0;
	}
	const Blocks::iterator& end = blocks_.get<by_any>().end();
	Blocks::iterator begin = blocks_.get<by_any>().find(
			boost::make_tuple(source,iteration,block));
	if(begin == end) return shared_ptr<Block>();
	return *begin;
}

int Variable::CountLocalBlocks(int iteration) const
{
	int istart = iteration;
	int iend = iteration;
	if(not IsTimeVarying()) {
		istart = 0;
		iend = 0;
	}
	BlocksByIteration::iterator begin 
		= blocks_.get<by_iteration>().lower_bound(istart);
	BlocksByIteration::iterator end 
		= blocks_.get<by_iteration>().upper_bound(iend);
	return std::distance(begin,end);
}

int Variable::CountTotalBlocks(int iteration) const
{
	// we assume every process holds the same number of blocks
	int nbrServer = Environment::CountTotalServers();
	return nbrServer*CountLocalBlocks(iteration);
}

bool Variable::DetachBlock(const shared_ptr<Block>& c)
{
	if(not c) return true;
        Blocks::iterator end 
		= blocks_.get<by_any>().end();
	
        Blocks::iterator it = blocks_.get<by_any>().find(
			boost::make_tuple(c->GetSource(),
					c->GetIteration(),
					c->GetID()));
	if(it != end) {
		blocks_.get<by_any>().erase(it);
		return true;
	} else {
		return false;
	}
}

void Variable::ClearAll()
{
	blocks_.get<by_any>().clear();
}

void Variable::ClearIteration(int iteration)
{
	BlocksByIteration::iterator begin 
		= blocks_.get<by_iteration>().lower_bound(iteration);
	BlocksByIteration::iterator end 
		= blocks_.get<by_iteration>().upper_bound(iteration);
	blocks_.get<by_iteration>().erase(begin,end);
}

void Variable::ClearUpToIteration(int iteration)
{
	BlocksByIteration::iterator begin
		= blocks_.get<by_iteration>().lower_bound(-1);
	BlocksByIteration::iterator end
		= blocks_.get<by_iteration>().upper_bound(iteration);
	blocks_.get<by_iteration>().erase(begin,end);
}

void Variable::ClearSource(int source)
{
	BlocksBySource::iterator begin 
		= blocks_.get<by_source>().lower_bound(source);
	BlocksBySource::iterator end 
		= blocks_.get<by_source>().upper_bound(source);
	blocks_.get<by_source>().erase(begin,end);
}

void Variable::ClearId(int id)
{
	BlocksById::iterator begin 
		= blocks_.get<by_id>().lower_bound(id);
	BlocksById::iterator end 
		= blocks_.get<by_id>().upper_bound(id);
	blocks_.get<by_id>().erase(begin,end);
}

shared_ptr<Block> Variable::Allocate(int source, int iteration, int bid, 
			bool blocking)
{
	if(not buffer_) {
		GetBuffer();
		if(not buffer_) return shared_ptr<Block>();
	}

	if((not IsTimeVarying()) && (iteration != 0)) {
		WARN("Trying to write a non time-varying variable at a "
		<< " non-0 iteration, will probably leave the simulation in "
		<< " an inconsistent state.");
		iteration = 0;
	}

	shared_ptr<Block> block = GetBlock(source,iteration,bid);
	if(block) {
		ERROR("Trying to overwrite an existing chunk for variable \""
			<< name_ << "\"");
		return shared_ptr<Block>();
	}

	block = Block::New(source,iteration,bid,SHARED_FROM_THIS());
	if(not block) {
		ERROR("When creating block");
		return block;
	}
	
	DataSpace<Buffer> ds;
	size_t size = GetLayout()->GetRequiredMemory();
	
	ds = buffer_->Allocate(size);
	if(ds.GetSize() != size) {
		if(blocking) {
			while(ds.GetSize() != size) {
				ds = buffer_->Allocate(size);
				buffer_->WaitAvailable(size);
			}
		} else {
			return shared_ptr<Block>();
		}
	}

	block->SetDataSpace(ds);
	
	if(positions_.count(bid)) {
		block->Move(positions_[bid]);
	}

	AttachBlock(block);

	return block;
}

shared_ptr<Block> Variable::AllocateFixedSize(int source, int iteration, int bid,
			const std::vector<int64_t>& lbounds,
			const std::vector<int64_t>& ubounds, 
			const std::vector<int64_t>& gbounds,
			const std::vector<size_t>& ghosts, 
			bool blocking)
{
	if(not buffer_) {
		GetBuffer();
		if(not buffer_) return shared_ptr<Block>();
	}

	if((not IsTimeVarying()) && (iteration != 0)) {
		WARN("Trying to write a non time-varying variable at a "
		<< " non-0 iteration, will probably leave the simulation in "
		<< " an inconsistent state.");
		iteration = 0;
	}

	shared_ptr<Block> block = GetBlock(source,iteration,bid);
	if(block) {
		ERROR("Trying to overwrite an existing chunk for variable \""
			<< name_ << "\"");
		return shared_ptr<Block>();
	}

	block = Block::New(source,iteration,bid,SHARED_FROM_THIS());
	if(not block) {
		ERROR("When creating block");
		return block;
	}

	for(unsigned int i=0; i<GetLayout()->GetDimensions(); i++) {
		block->SetStartIndex(i,lbounds[i]);
		block->SetEndIndex(i,ubounds[i]);
		block->global_dims_[i] = gbounds[i];
		block->ghosts_[i] = std::make_pair(ghosts[2*i],ghosts[2*i+1]);
	}

	DataSpace<Buffer> ds;
	//size_t size = GetLayout()->GetRequiredMemory();
	size_t size = block->NbrOfItems();
	size *= TypeSize(GetLayout()->GetType());

	ds = buffer_->Allocate(size);
	if(ds.GetSize() != size) {
		if(blocking) {
			while(ds.GetSize() != size) {
				ds = buffer_->Allocate(size);
				buffer_->WaitAvailable(size);
			}
		} else {
			return shared_ptr<Block>();
		}
	}

	block->SetDataSpace(ds);

/*	if(positions_.count(bid)) {
		block->Move(positions_[bid]);
	}
*/

	AttachBlock(block);

	return block;
}

shared_ptr<Block> Variable::AllocateAligned(int source, int iteration, int bid, 
			bool blocking)
{
	if(not buffer_) {
		GetBuffer();
		if(not buffer_) return shared_ptr<Block>();
	}

	if((not IsTimeVarying()) && (iteration != 0)) {
		WARN("Trying to write a non time-varying variable at a "
		<< " non-0 iteration, will probably leave the simulation in "
		<< " an inconsistent state.");
		iteration = 0;
	}

	shared_ptr<Block> block = GetBlock(source,iteration,bid);
	if(block) {
		ERROR("Trying to overwrite an existing chunk for variable \""
			<< name_ << "\"");
		return shared_ptr<Block>();
	}

	block = Block::New(source,iteration,bid,SHARED_FROM_THIS());
	if(not block) {
		ERROR("When creating block");
		return block;
	}
	
	DataSpace<Buffer> ds;
	size_t size = GetLayout()->GetRequiredMemory();
	
	ds = buffer_->AllocateAligned(size);
	if(ds.GetSize() != size) {
		if(blocking) {
			while(ds.GetSize() != size) {
				ds = buffer_->AllocateAligned(size);
				buffer_->WaitAvailable(size);
			}
		} else {
			return shared_ptr<Block>();
		}
	}

	block->SetDataSpace(ds);
	
	if(positions_.count(bid)) {
		block->Move(positions_[bid]);
	}

	AttachBlock(block);

	return block;
}

shared_ptr<Block> Variable::Retrieve(int source, int iteration, int bid,
					const Handle& h, size_t size)
{
	if(not buffer_) {
		GetBuffer();
		if(not buffer_) return shared_ptr<Block>();
	}
	
	shared_ptr<Block> existing = GetBlock(source,iteration,bid);
	if(existing) {
		ERROR("Retrieving an already existing Block. "
			<< "The new one will not be considered. "
			<< "Possible memory leak from this point.");
		return shared_ptr<Block>();
	}
	DBG("Block retrieved for iteration " 
		<< iteration << " block " << bid);
	
	void* addr = buffer_->GetAddressFromHandle(h);
	
	DataSpace<Buffer> ds;
	if(addr == NULL) {
		return shared_ptr<Block>();
	}
	shared_ptr<Block> block 
		= Block::New(source,iteration,bid,SHARED_FROM_THIS());
	
	ds.Link(buffer_,addr,size);
	block->SetDataSpace(ds);
	AttachBlock(block);
	
	block->LoseDataOwnership();
	
	if(positions_.count(bid)) {
		block->Move(positions_[bid]);
	}
	
	return block;
}

shared_ptr<Block> Variable::Retrieve(int source, int iteration, int bid,
					const std::vector<int64_t>& lbounds,
					const std::vector<int64_t>& ubounds,
					const std::vector<int64_t>& gbounds,
					const std::vector<size_t>&  ghosts,
					const Handle& h)
{
	if(lbounds.size() != GetLayout()->GetDimensions()
	|| ubounds.size() != GetLayout()->GetDimensions()) {
		return shared_ptr<Block>();
	}
	
	size_t size = TypeSize(GetLayout()->GetType());
	for(unsigned int i=0; i<lbounds.size(); i++) {
		size *= (ubounds[i] - lbounds[i] + 1);
	}
	
	shared_ptr<Block> b = Retrieve(source,iteration,bid,h,size);
	if(not b) return b;
	
	if((int)lbounds.size() != b->GetDimensions()
	|| (int)ubounds.size() != b->GetDimensions()) {
		ERROR("Input ubound and lbound vectors have invalid dimensions");
		return b;
	}
	
	for(int i = 0; i < b->GetDimensions(); i++) {
		b->SetStartIndex(i,lbounds[i]);
		b->SetEndIndex(i,ubounds[i]);
		b->global_dims_[i] = gbounds[i];
		b->ghosts_[i] = std::make_pair(ghosts[2*i],ghosts[2*i+1]);
	}
	
	return b;
}

void Variable::GetBlocksByIteration(int iteration,
	BlocksByIteration::iterator& begin,
	BlocksByIteration::iterator& end) const
{
	begin = blocks_.get<by_iteration>().lower_bound(iteration);
	end = blocks_.get<by_iteration>().upper_bound(iteration);
}

void Variable::GetBlocksBySource(int source,
	BlocksBySource::iterator& begin,
	BlocksBySource::iterator& end) const
{
	begin = blocks_.get<by_source>().lower_bound(source);
	end = blocks_.get<by_source>().upper_bound(source);
}

void Variable::GetBlocksById(int bid,
	BlocksById::iterator& begin,
	BlocksById::iterator& end) const
{
	begin = blocks_.get<by_id>().lower_bound(bid);
	end = blocks_.get<by_id>().upper_bound(bid);
}

bool Variable::GetIterationRange(int& lowest, int& biggest) const
{
	if(blocks_.size() == 0)
		return false;

	lowest  = blocks_.get<by_iteration>().begin()->get()->GetIteration();
	biggest = blocks_.get<by_iteration>().rbegin()->get()->GetIteration();
	return true;
}

bool Variable::GetSourceRange(int& lowest, int& biggest) const 
{
	if(blocks_.size() == 0)
		return false;

	lowest  = blocks_.get<by_source>().begin()->get()->GetSource();
	biggest = blocks_.get<by_source>().rbegin()->get()->GetSource();
	return true;
}

bool Variable::GetIDRange(int& lowest, int& biggest) const 
{
	if(blocks_.size() == 0)
		return false;

	lowest  = blocks_.get<by_id>().begin()->get()->GetSource();
	biggest = blocks_.get<by_id>().rbegin()->get()->GetSource();
	return true;
}

#ifdef HAVE_VISIT_ENABLED
bool Variable::ExposeVisItMetaData(visit_handle md, int UNUSED(iteration))
{
	if((not GetModel().visualizable()) 
		|| (GetModel().mesh() == "#")) {
		return false;
	}

	visit_handle vmd = VISIT_INVALID_HANDLE;
	if(VisIt_VariableMetaData_alloc(&vmd) == VISIT_OKAY) {
		VisIt_VariableMetaData_setName(vmd, GetName().c_str());
		if(GetModel().mesh() != "#") {
			VisIt_VariableMetaData_setMeshName(vmd, 
				GetModel().mesh().c_str());
		}
		VisIt_VariableMetaData_setType(vmd, 
			VarTypeToVisIt(GetModel().type()));
		VisIt_VariableMetaData_setCentering(vmd, 
			VarCenteringToVisIt(GetModel().centering()));

		VisIt_SimulationMetaData_addVariable(md, vmd);
		return true;
	}
	DBG("Unable to allocate VisIt handle for variable \"" << GetName() << "\"");
	return false;
}

bool Variable::ExposeVisItDomainList(visit_handle *h, int UNUSED(iteration))
{
	if(VisIt_DomainList_alloc(h) != VISIT_ERROR)
	{
		visit_handle hdl;
		int *iptr = NULL;

		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalClients = Environment::HasServer() ? 
				Environment::ClientsPerNode() : 1;
		int nbrBlocksPerClient = Environment::NumDomainsPerClient();
		int nbrBlocks = nbrLocalClients*nbrBlocksPerClient;
		int ttlClients = Environment::CountTotalClients();
		int ttlBlocks = ttlClients*nbrBlocksPerClient;

		DBG("nbrLocalClients = " << nbrLocalClients 
		<< " ttlClients = " << ttlClients);

		std::list<int>::const_iterator it = clients.begin();
		iptr = (int *)malloc(sizeof(int)*nbrBlocks);
		for(int i = 0; i < nbrLocalClients; i++) {
			for(int j = 0; j < nbrBlocksPerClient; j++) {
				iptr[i*nbrBlocksPerClient + j] 
					= (*it)*nbrBlocksPerClient + j;
			}
			it++;
		}

		if(VisIt_VariableData_alloc(&hdl) == VISIT_OKAY)
		{
			VisIt_VariableData_setDataI(hdl, VISIT_OWNER_VISIT, 1, 
					nbrBlocks, iptr);
			VisIt_DomainList_setDomains(*h, ttlBlocks, hdl);
			return true;
		} else {
			free(iptr);
		}
	}
	return false;
}

bool Variable::ExposeVisItData(visit_handle* h, 
	int source, int iteration, int block)
{
	shared_ptr<Block> b = GetBlock(source,iteration,block);
	if(not b) {
		*h = VISIT_INVALID_HANDLE;
		return true;
	} else {
		if(VisIt_VariableData_alloc(h) == VISIT_OKAY) {
			b->FillVisItDataHandle(*h);
			return true;
		} else {
			ERROR("While allocating VisIt handle");
		}
	}
	return false;
}

VisIt_VarType Variable::VarTypeToVisIt(const model::VarType& vt) 
{
	switch(vt) {
		case model::VarType::scalar :
			return VISIT_VARTYPE_SCALAR;
		case model::VarType::vector :
			return VISIT_VARTYPE_VECTOR;
		case model::VarType::tensor :
			return VISIT_VARTYPE_TENSOR;
		case model::VarType::symmetric_tensor :
			return VISIT_VARTYPE_SYMMETRIC_TENSOR;
		case model::VarType::material :
			return VISIT_VARTYPE_MATERIAL;
		case model::VarType::matspecies :
			return VISIT_VARTYPE_MATSPECIES;
		case model::VarType::label :
			return VISIT_VARTYPE_LABEL;
		case model::VarType::array :
			return VISIT_VARTYPE_ARRAY;
		case model::VarType::mesh :
			return VISIT_VARTYPE_MESH;
		case model::VarType::curve :
			return VISIT_VARTYPE_CURVE;
	}
	return VISIT_VARTYPE_SCALAR;
}

VisIt_VarCentering Variable::VarCenteringToVisIt(const model::VarCentering& vc) 
{
	if(vc == model::VarCentering::zonal) {
		return VISIT_VARCENTERING_ZONE;
	} else {
		return VISIT_VARCENTERING_NODE;
	}
}

#endif

}
