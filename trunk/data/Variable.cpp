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
#include "core/LayoutManager.hpp"
#include "data/Variable.hpp"
#include "core/Debug.hpp"

namespace Damaris {

Variable::Variable(const Model::Variable& mdl, const std::string &n, Layout* l)
: Configurable<Model::Variable>(mdl)
{
	layout = l;
	name = n;
}

void Variable::attachChunk(Chunk* chunk)
{
	if(not model.time_varying()) {
		chunk->setIteration(0);
	}
	chunks.insert(boost::shared_ptr<Chunk>(chunk));
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

ChunkIndex::iterator Variable::getChunks(int source, int iteration, ChunkIndex::iterator &end)
{
	if(not model.time_varying()) {
		iteration = 0;
	}
	end = chunks.get<by_any>().end();
	return chunks.get<by_any>().find(boost::make_tuple(source,iteration));
}

void Variable::detachChunk(ChunkIndexByIteration::iterator &it)
{
	chunks.get<by_iteration>().erase(it);
}

void Variable::eraseChunk(ChunkIndexByIteration::iterator &it)
{
	it->get()->remove();
	detachChunk(it);
}

void Variable::detachChunk(ChunkIndexBySource::iterator &it)
{
    chunks.get<by_source>().erase(it);
}

void Variable::eraseChunk(ChunkIndexBySource::iterator &it)
{
	it->get()->remove();
	detachChunk(it);
}

void Variable::clear()
{
	ChunkIndexBySource::iterator it = chunks.get<by_source>().begin();
	while(it != chunks.get<by_source>().end())
	{
		it->get()->remove();
		it++;
	}
	chunks.get<by_source>().clear();
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

bool Variable::exposeVisItData(visit_handle* h, int source, int iteration)
{
	if(VisIt_VariableData_alloc(h) == VISIT_OKAY) {
		ChunkIndex::iterator end;
		ChunkIndex::iterator it = getChunks(source, iteration, end);
		if(it == end) {
			VisIt_VariableData_free(*h);
			return false;
		}
		(*it)->FillVisItDataHandle(*h);
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

}
