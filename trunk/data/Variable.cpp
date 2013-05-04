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
 * \file Variable.cpp
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include <iterator>
#include "core/Process.hpp"
#include "core/LayoutManager.hpp"
#include "data/Variable.hpp"
#include "data/ChunkDescriptor.hpp"
#include "data/ChunkHeader.hpp"
#include "core/Debug.hpp"

namespace Damaris {

Variable::Variable(const Model::Variable& mdl, const std::string &n, Layout* l)
: Configurable<Model::Variable>(mdl)
{
	layout = l;
	name = n;
	allocator = NULL;
}

bool Variable::AttachChunk(Chunk* chunk)
{
	if(not model.time_varying()) {
		chunk->SetIteration(0);
	}
	return chunks.insert(boost::shared_ptr<Chunk>(chunk)).second;
}

Chunk* Variable::GetChunk(int source, int iteration, int block)
{
	if(not model.time_varying()) {
		iteration = 0;
	}
	const ChunkIndex::iterator& end = chunks.get<by_any>().end();
	ChunkIndex::iterator begin = chunks.get<by_any>().find(
			boost::make_tuple(source,iteration,block));
	if(begin == end) return NULL;
	return begin->get();
}

int Variable::CountLocalBlocks(int iteration) const
{
	int istart = iteration;
	int iend = iteration;
	if(not model.time_varying()) {
		istart = 0;
		iend = 0;
	}
	ChunkIndexByIteration::iterator begin = chunks.get<by_iteration>().lower_bound(istart);
	ChunkIndexByIteration::iterator end = chunks.get<by_iteration>().upper_bound(iend);
	return std::distance(begin,end);
}

int Variable::CountTotalBlocks(int iteration) const
{
	// we assume every process holds the same number of blocks
	int nbrServer = Environment::CountTotalServers();
	return nbrServer*CountLocalBlocks(iteration);
}

Position* Variable::GetBlockPosition(int block_id) 
{
	if(positions.count(block_id) == 0) {
		positions[block_id] = boost::shared_ptr<Position>(new Position());
	}
	return positions[block_id].get();
}

bool Variable::DetachChunk(Chunk* c)
{
	int iteration = c->GetIteration();
	int source = c->GetSource();
	int block = c->GetBlock();

	if(not model.time_varying()) {
                iteration = 0;
        }
        ChunkIndex::iterator end = chunks.get<by_any>().end();
        ChunkIndex::iterator it = chunks.get<by_any>().find(
					boost::make_tuple(source,iteration,block));

	while(it != end) {
		if(it->get() == c) {
			it = chunks.get<by_any>().erase(it);
			return true;
			break;
		}
		it++;
	}
	return false;
}

Variable::iterator Variable::DetachChunk(Variable::iterator& it)
{
	return chunks.erase(it);
}

void Variable::ClearAll()
{
	chunks.get<by_any>().clear();
}

void Variable::Clear(int iteration)
{
	iterator it = chunks.begin();
	iterator end = chunks.end();
	while(it != end) {
		if(it->get()->GetIteration() == iteration) {
			it = DetachChunk(it);
		} else {
			it++;
		}
	}
}

#ifdef __ENABLE_VISIT
bool Variable::ExposeVisItMetaData(visit_handle md, int iteration)
{
	if((not model.visualizable()) || (model.mesh() == "#")) {
		return false;
	}

	visit_handle vmd = VISIT_INVALID_HANDLE;
	if(VisIt_VariableMetaData_alloc(&vmd) == VISIT_OKAY) {
		VisIt_VariableMetaData_setName(vmd, name.c_str());
		if(model.mesh() != "#") {
			VisIt_VariableMetaData_setMeshName(vmd, model.mesh().c_str());
		}
		VisIt_VariableMetaData_setType(vmd, VarTypeToVisIt(model.type()));
		VisIt_VariableMetaData_setCentering(vmd, VarCenteringToVisIt(model.centering()));

		VisIt_SimulationMetaData_addVariable(md, vmd);
		return true;
	}
	DBG("Unable to allocate VisIt handle for variable \"" << name << "\"");
	return false;
}

bool Variable::ExposeVisItDomainList(visit_handle *h, int iteration)
{
	if(VisIt_DomainList_alloc(h) != VISIT_ERROR)
	{
		visit_handle hdl;
		int *iptr = NULL;

		std::list<int> clients = Environment::GetKnownLocalClients();
		int nbrLocalClients = Environment::HasServer() ? Environment::ClientsPerNode() : 1;
		int nbrBlocksPerClient = Environment::NumDomainsPerClient();
		int nbrBlocks = nbrLocalClients*nbrBlocksPerClient;
		int ttlClients = Environment::CountTotalClients();
		int ttlBlocks = ttlClients*nbrBlocksPerClient;

		DBG("nbrLocalClients = " << nbrLocalClients << " ttlClients = " << ttlClients);

		std::list<int>::const_iterator it = clients.begin();
		iptr = (int *)malloc(sizeof(int)*nbrBlocks);
		for(int i = 0; i < nbrLocalClients; i++) {
			for(int j = 0; j < nbrBlocksPerClient; j++) {
				iptr[i*nbrBlocksPerClient + j] = (*it)*nbrBlocksPerClient + j;
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

bool Variable::ExposeVisItData(visit_handle* h, int source, int iteration, int block)
{
	DBG("source = " << source << ", iteration = " << iteration);
	Chunk* chunk = GetChunk(source,iteration,block);
	if(chunk == NULL) {
		*h = VISIT_INVALID_HANDLE;
		return true;
	} else {
		if(VisIt_VariableData_alloc(h) == VISIT_OKAY) {
			chunk->FillVisItDataHandle(*h);
			return true;
		} else {
			ERROR("While allocating VisIt handle");
		}
	}
	return false;
}

VisIt_VarType Variable::VarTypeToVisIt(const Model::VarType& vt) 
{
	switch(vt) {
		case Model::VarType::scalar :
			return VISIT_VARTYPE_SCALAR;
		case Model::VarType::vector :
			return VISIT_VARTYPE_VECTOR;
		case Model::VarType::tensor :
			return VISIT_VARTYPE_TENSOR;
		case Model::VarType::symmetric_tensor :
			return VISIT_VARTYPE_SYMMETRIC_TENSOR;
		case Model::VarType::material :
			return VISIT_VARTYPE_MATERIAL;
		case Model::VarType::matspecies :
			return VISIT_VARTYPE_MATSPECIES;
		case Model::VarType::label :
			return VISIT_VARTYPE_LABEL;
		case Model::VarType::array :
			return VISIT_VARTYPE_ARRAY;
		case Model::VarType::mesh :
			return VISIT_VARTYPE_MESH;
		case Model::VarType::curve :
			return VISIT_VARTYPE_CURVE;
	}
	return VISIT_VARTYPE_SCALAR;
}

VisIt_VarCentering Variable::VarCenteringToVisIt(const Model::VarCentering& vc) 
{
	if(vc == Model::VarCentering::zonal) {
		return VISIT_VARCENTERING_ZONE;
	} else {
		return VISIT_VARCENTERING_NODE;
	}
}

#endif

Variable* Variable::New(const Model::Variable& mdl, const std::string& name)
{
	// checks that the layout exists
	Layout* l = LayoutManager::Search((std::string)mdl.layout());
	if(l == NULL) {
		CFGERROR("Layout \"" << mdl.layout() 
				<< "\" not found for variable \"" 
				<< mdl.name() << "\"");
		return NULL;
	}
	return new Variable(mdl,name,l);
}

Chunk* Variable::Allocate(int block, bool blocking)
{
	if(allocator == NULL) {
		allocator = Process::Get()->getSharedMemorySegment();
	}
	int iteration = Environment::GetLastIteration();
	int source = Process::Get()->getID();

	if((not IsTimeVarying()) && (iteration != 0)) {
		WARN("Trying to write a non time-varying variable at a "
		<< " non-0 iteration, will probably leave the simulation in "
		<< " an inconsistent state.");
		iteration = 0;
	}

	Chunk* chunk = GetChunk(source,iteration,block);
	if(chunk != NULL) {
		ERROR("Trying to overwrite an existing chunk for variable \""
			<< name << "\"");
		return NULL;
	}

	ChunkDescriptor* cd = ChunkDescriptor::New(*layout);
	if(positions.count(block) == 1) {
		Position* p = GetBlockPosition(block);
		cd->Move(p);
	}
	size_t size = sizeof(ChunkHeader)+cd->GetDataMemoryLength(layout->GetType());
	void* location = allocator->Allocate(size);

	if((location == NULL) && (not blocking)) {
		DBG("Could not allocate memory for variable \"" 
		<< name << "\": not enough memory");
		ChunkDescriptor::Delete(cd);
		return NULL;
	} else if((location == NULL) && blocking) {
		while(location == NULL) {
			if(allocator->WaitAvailable(size)) {
				location = allocator->Allocate(size);
			} else {
				DBG("Could not allocate memory for variable \""
				<< name << "\": not enough memory");
			}
		}
	}

	
	ChunkHeader* ch =
		new(location) ChunkHeader(cd,layout->GetType(),
					iteration,source, block);
	
	chunk = new Chunk(allocator,ch);
	AttachChunk(chunk);

	ChunkDescriptor::Delete(cd);
	return chunk;
}

Chunk* Variable::Retrieve(void* addr)
{
	handle_t h = allocator->GetHandleFromAddress(addr);
	return Retrieve(h);
}

Chunk* Variable::Retrieve(handle_t h)
{
	allocator = Process::Get()->getSharedMemorySegment();

	Chunk* chunk = new Chunk(allocator,h);

	int iteration = chunk->GetIteration();
	int source = chunk->GetSource();
	int block = chunk->GetBlock();

	Chunk* existing = GetChunk(source,iteration,block);
	if(existing != NULL) {
		if(existing->Data() == chunk->Data()) {
			delete chunk;
			return existing;
		} else {
			ERROR("Retrieving an already existing Chunk. The new one will not be considered.");
			chunk->SetDataOwnership(true);
			delete chunk;
			return existing;
		}
	}
	DBG("Chunk retrieved for iteration " << iteration << " block " << block);
	chunk->SetDataOwnership(true);
	AttachChunk(chunk);
	DBG("Number of stored chunks: " << chunks.size());
	return chunk;
}

Variable::iterator Variable::Begin() 
{
	return chunks.begin();
}

Variable::iterator Variable::End()
{
	return chunks.end();
}
}
