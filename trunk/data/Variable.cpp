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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Process.hpp"
#include "core/LayoutManager.hpp"
#include "data/Variable.hpp"
#include "data/ChunkDescriptor.hpp"
#include "data/ChunkHeader.hpp"
#include "data/ChunkImpl.hpp"
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
		chunk->setIteration(0);
	}
	return chunks.insert(boost::shared_ptr<Chunk>(chunk)).second;
}

ChunkIndexBySource::iterator Variable::getChunksBySource(int source,
	ChunkIndexBySource::iterator& end)
{
	ChunkIndexBySource::iterator it = chunks.get<by_source>().lower_bound(source);
	end = chunks.get<by_source>().upper_bound(source);
	return it;
}

ChunkIndexByIteration::iterator Variable::getChunksByIteration(int iteration,
	ChunkIndexByIteration::iterator& end) 
{
	return getChunksByIterationsRange(iteration,iteration,end);
}

ChunkIndexByIteration::iterator Variable::getChunksByIterationsRange(int itstart, int itend,
            ChunkIndexByIteration::iterator& end)
{
	if(not model.time_varying()) {
		itstart = 0;
		itend = 0;
	}
	ChunkIndexByIteration::iterator it = chunks.get<by_iteration>().lower_bound(itstart);
    end = chunks.get<by_iteration>().upper_bound(itend);
    return it;
}

ChunkIndex::iterator Variable::getChunks(ChunkIndex::iterator &end)
{
	end = chunks.get<by_any>().end();
	return chunks.get<by_any>().begin();
}

ChunkIndex::iterator Variable::getChunks(int source, int iteration, int block, ChunkIndex::iterator &end)
{
	if(not model.time_varying()) {
		iteration = 0;
	}
	end = chunks.get<by_any>().end();
	return chunks.get<by_any>().find(boost::make_tuple(source,iteration,block));
}

Chunk* Variable::GetChunk(int source, int iteration, int block)
{
	if(not model.time_varying()) {
		iteration = 0;
	}
	ChunkIndex::iterator end = chunks.get<by_any>().end();
	ChunkIndex::iterator begin = chunks.get<by_any>().find(boost::make_tuple(source,iteration,block));
	if(begin == end) return NULL;
	return begin->get();
}

bool Variable::DetachChunk(Chunk* c)
{
	int iteration = c->getIteration();
	int source = c->getSource();
	int block = c->getBlock();

	if(not model.time_varying()) {
                iteration = 0;
        }
        ChunkIndex::iterator end = chunks.get<by_any>().end();
        ChunkIndex::iterator it = chunks.get<by_any>().find(
					boost::make_tuple(source,iteration,block));

	while(it != end) {
		if(it->get() == c) {
			chunks.get<by_any>().erase(it);
			return true;
			break;
		}
		it++;
	}
	return false;
}

void Variable::ClearAll()
{
	chunks.get<by_any>().clear();
}

#ifdef __ENABLE_VISIT
bool Variable::exposeVisItMetaData(visit_handle md)
{
	if(not model.visualizable()) {
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
	ERROR("Unable to allocate VisIt handle for variable \"" << name << "\"");
	return false;
}

bool Variable::exposeVisItData(visit_handle* h, int source, int iteration, int block)
{
	DBG("source = " << source << ", iteration = " << iteration);
	if(VisIt_VariableData_alloc(h) == VISIT_OKAY) {
		//ChunkIndex::iterator end;
		//ChunkIndex::iterator it = getChunks(source, iteration, end);
		Chunk* chunk = GetChunk(source,iteration,block);
		if(chunk == NULL) {
			ERROR("Chunk not found for source = " << source
				<< ", iteration = " << iteration << ", block = " << block);
			VisIt_VariableData_free(*h);
			return false;
		}
		chunk->FillVisItDataHandle(*h);
		return true;
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

Chunk* Variable::Allocate(int block)
{
	if(allocator == NULL) {
		allocator = Process::get()->getSharedMemorySegment();
	}
	int iteration = Environment::GetLastIteration() + 1;
	int source = Process::get()->getID();

	if(not IsTimeVarying()) {
		iteration = 0;
	}

	Chunk* chunk = GetChunk(source,iteration,block);
	if(chunk != NULL) {
		ERROR("Trying to overwrite an existing chunk for variable \""
			<< name << "\"");
		return NULL;
	}

	ChunkDescriptor* cd = ChunkDescriptor::New(*layout);
	size_t size = sizeof(ChunkHeader)+cd->getDataMemoryLength(layout->getType());
	void* location = allocator->allocate(size);

	if(location == NULL) {
		ERROR("Could not allocate memory for variable \"" 
			<< name << "\": not enough memory");
		ChunkDescriptor::Delete(cd);
		return NULL;
	}

	ChunkHeader* ch = 
		new(location) ChunkHeader(cd,layout->getType(),
					iteration,source, block);
	
	chunk = new ChunkImpl(allocator,ch);
	AttachChunk(chunk);

	ChunkDescriptor::Delete(cd);
	return chunk;
}

Chunk* Variable::Retrieve(void* addr)
{
	handle_t h = allocator->getHandleFromAddress(addr);
	return Retrieve(h);
}

Chunk* Variable::Retrieve(handle_t h)
{
	allocator = Process::get()->getSharedMemorySegment();

	ChunkImpl* chunk = new ChunkImpl(allocator,h);

	int iteration = chunk->getIteration();
	int source = chunk->getSource();
	int block = chunk->getBlock();

	Chunk* existing = GetChunk(source,iteration,block);
	if(existing != NULL) {
		if(existing->data() == chunk->data()) {
			delete chunk;
			return existing;
		} else {
			ERROR("Retrieving an already existing Chunk. The new one will not be considered.");
			chunk->SetDataOwnership(true);
			delete chunk;
			return existing;
		}
	}
	chunk->SetDataOwnership(true);
	AttachChunk(chunk);
	return chunk;
}

}
