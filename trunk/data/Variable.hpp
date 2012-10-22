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
 * \file Variable.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <stdint.h>
#include <list>
#include <string>

#ifdef __ENABLE_VISIT
#include <VisItDataInterface_V2.h>
#endif

#include "xml/Model.hpp"
#include "core/Configurable.hpp"
#include "memory/Buffer.hpp"
#include "data/ChunkSet.hpp"
#include "data/Layout.hpp"
#include "core/Manager.hpp"

namespace Damaris {

/**
 * The Variable object is used for describing a variable within
 * a metadata structure. It holds an index of chunks and additional
 * informations. A Variable record is identified by a name and an ID.
 * A Variable cannot be created in a vacuum, only the MetadataManager
 * has the permission to create instances of them.
 */
class Variable : public Configurable<Model::Variable> {

	friend class Manager<Variable>;
	
	private:	
		Layout* layout;		/*!< Layout of the data. */
		ChunkSet chunks; 	/*!< Chunks hold by the variable. */
		Buffer* allocator;	/*!< Allocator to use for chunks. */
		std::string name; 	/*!< Name of the variable. */
		int id;			/*!< id of the variable, given by the VariableManager. */
	
		/**
		 * \brief Constructor. 
		 * Creates a Variable record given a model (from XML), a name and a layout.
		 * The constructor is private, use New to create an instance.
		 */
		Variable(const Model::Variable& v, const std::string& name, Layout* l);

	public:
		
		typedef typename ChunkSet::iterator iterator;
	
		/**
		 * Returns the layout of the variable.
		 */
		Layout* GetLayout() const { return layout; }
	
		/**
		 * Returns the name of the variable.
		 */
		const std::string& GetName() const { return name; }

		/**
		 * Returns the id of the variable, as given by the VariableManager.
		 */
		int GetID() const { return id; }

		/**
		 * Returns the allocator used for chunks allocation.
		 */
		Buffer* Allocator() const { return allocator; }

		/**
		 * Attach a new chunk to the variable.
		 * Returns true if the Chunk has been attached, false otherwise.
		 */
		bool AttachChunk(Chunk* chunk);

		/**
		 * Returns the list of chunks with a specified source.
		 * \deprecated
		 */
		ChunkIndexBySource::iterator getChunksBySource(int source,
			ChunkIndexBySource::iterator& end)
		__attribute__ ((deprecated));

		/**
		 * Returns the list of chunks with a specified iteration.
		 * \deprecated
		 */
		ChunkIndexByIteration::iterator getChunksByIteration(int iteration,
			ChunkIndexByIteration::iterator& end)
		__attribute__ ((deprecated));

		/**
		 * Returns an iterator of chunks with an iteration within a given range.
		 * \deprecated
		 */
		ChunkIndexByIteration::iterator getChunksByIterationsRange(int itstart, int itend,
			ChunkIndexByIteration::iterator& end)
		__attribute__ ((deprecated));
		
		/**
		 * Returns an iterator over all the chunks.
		 * \param[out] end : a reference that will hold the end of the iterator.
		 * \deprecated
		 */
		ChunkIndex::iterator getChunks(ChunkIndex::iterator &end)
		__attribute__ ((deprecated));

		/**
		 * Finds a Chunk of a given source, iteration and block.
		 * Returns NULL if this chunk doesn't exist.
		 */
		Chunk* GetChunk(int source, int iteration, int block=0);

		/**
		 * Counts the number of blocks for a given iteration.
		 */
		int CountLocalBlocks(int iteration) const;

		/**
		 * Counts the total number of blocks for a given iteration.
		 */
		int CountTotalBlocks(int iteration) const;

		/**
		 * Returns an iterator over all the chunks that correspond to a given
		 * source and iteration.
		 * \param[in] source : source wanted.
		 * \param[in] iteration : iteration wanted.
		 * \param[in] block : block id wanted.
		 * \param[out] end : a reference that will hold the end of the iterator.
		 * \deprecated
		 */
		ChunkIndex::iterator getChunks(int source, int iteration, int block, 
			ChunkIndex::iterator &end) __attribute__ ((deprecated));

		/**
		 * Detach a chunk from a variable. Free its memory if the process owns the chunk.
		 * To prevent from loosing data, don't forget to unset the chunk's ownership before
		 * detaching the chunk.
		 */
		bool DetachChunk(Chunk* c);

		/**
		 * Detach a chunk from a variable, for use in a loop using iterators.
		 */
		iterator DetachChunk(iterator& it);

		/**
		 * Delete all the chunks held by a Variable.
		 */
		void ClearAll();

		/**
		 * Returns the variables description.
		 */
		std::string GetDescription() const { return (std::string)model; }

		/**
		 * Returns the unit of the variable.
		 */
		std::string GetUnit() const { return model.unit(); }

		/**
		 * Return true if the variable is time-varying.
		 */
		bool IsTimeVarying() const { return model.time_varying(); }

#ifdef __ENABLE_VISIT
		/**
		 * Fills VisIt's metadata handle with information related to the variable.
		 */
		bool ExposeVisItMetaData(visit_handle md, int iteration);

		/**
		 * Fills VisIt's data handle with the proper data.
		 */
		bool ExposeVisItData(visit_handle *md, int source, int iteration, int block);

		/**
		 * Fills a VisIt handle to expose the domain list.
		 */
		bool ExposeVisItDomainList(visit_handle *md, int iteration);

	private:
		/**
		 * Converts a Model::VarCentering enum to a VisIt_VarCentering enum.
		 */
		static VisIt_VarCentering VarCenteringToVisIt(const Model::VarCentering& vc);
		
		/**
		 * Converts a Model::VarType enum to a VisIt_VarType enum.
		 */
		static VisIt_VarType VarTypeToVisIt(const Model::VarType& vt);
#endif

	public:
		/**
		 * Allocates a Chunk of a given block id.
		 * The current iteration and the source id are retrieved
		 * from the Environment and Process classes.
		 */
		virtual Chunk* Allocate(int block);

		/**
		 * Retrieves a Chunk from a memory pointer.
		 */
		virtual Chunk* Retrieve(void* addr);

		/**
		 * Retrieves a Chunk from a handle to the internal allocator.
		 */
		virtual Chunk* Retrieve(handle_t h);

		/**
		 * Iterates on all Chunks stored.
		 * No order can be assumed.
		 */
		template<typename F>
		void ForEach(F& f);

		/**
		 * Iterates on all Chunks stored that satisfy a given condition.
		 */
		template<typename F, typename C>
		void ForEach(F& f, C& c);

		/**
		 * Returns an iterator to the list of stored chunks
		 */
		iterator Begin();

		/**
		 * Returns an iterator to the end of the stored chunks
		 */
		iterator End();

		/**
		 * Creates an instance of Variable if the provided model is consistant.
		 */
		static Variable* New(const Model::Variable& mdl, const std::string &name);
};

template<typename F>
void Variable::ForEach(F& f)
{
	Variable::iterator it = chunks.begin();
	Variable::iterator end = chunks.end();
	while(it != end) {
		Variable::iterator copy = it;
		f(it,end,it->get());
		if(it == copy) it++;
	}
}

template<typename F, typename C>
void Variable::ForEach(F& f, C& c)
{
	Variable::iterator it = chunks.begin();
	Variable::iterator end = chunks.end();
	while(it != end) {
		if(c(it->get())) {
			Variable::iterator copy = it;
			f(it,end,it->get());
			if(it == copy) it++;
		} else {
			it++;
		}
	}
}

}

#endif
