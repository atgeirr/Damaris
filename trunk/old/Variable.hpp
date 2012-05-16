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

#include <VisItDataInterface_V2.h>

#include "common/ChunkSet.hpp"
#include "common/Layout.hpp"

namespace Damaris {

class MetadataManager;

/**
 * The Variable object is used for describing a variable within
 * a metadata structure. It holds an index of chunks and additional
 * informations. A Variable record is identified by a name and an ID.
 * A Variable cannot be created in a vacuum, only the MetadataManager
 * has the permission to create instances of them.
 */
class Variable {
	friend class MetadataManager;
	
	private:	
		std::string name;	/*!< Name of the variable. */
		std::string description; /*!< Description of the variable. */
		std::string unit;	 /*!< Unit of the variable. */
		Layout* layout;		/*!< Layout of the data. */
		int id;			/*!< The ID is used to avoid 
					  passing variable's name in shared-memory. */
		ChunkSet chunks; 	/*!< Chunks hold by the variable. */

		/**
		 * \brief Constructor. 
		 * Creates a Variable record given a name, an iteration, a source, a layout
		 * and a pointer to the data.
		 * The constructor is private, only a MetadataManager has the permission
		 * to create a new instance of Variable.
		 * 
		 * \param[in] vname : Name of the variable.
		 * \param[in] id : id of the variable in the MetadataManager.
		 * \param[in] layout : Layout that physically describe the data.
		 */
		Variable(int id, const std::string& vname, Layout* layout);

		/**
		 * \brief Constructor.
		 */
		Variable();
	
	public:	

		/**
		 * Returns the layout of the variable.
		 */
		Layout* getLayout() const { return layout; }
	
		/**
		 * Returns the name of the variable.
		 */
		const std::string& getName() const { return name; }

		/**
		 * Returns the ID of the variable.
		 */
		int getID() const { return id; }

		/**
		 * Attach a new chunk to the variable.
		 */
		void attachChunk(Chunk* chunk);

		/**
		 * \brief Comparison operator between variables.
		 * Variables are equals if they have a same name, iteration and source.
		 */
		bool operator==(const Variable &another);

		/**
		 * Returns the list of chunks with a specified source.
		 */
		ChunkIndexBySource::iterator getChunksBySource(int source,
			ChunkIndexBySource::iterator& end);

		/**
		 * Returns the list of chunks with a specified iteration.
		 */
		ChunkIndexByIteration::iterator getChunksByIteration(int iteration,
			ChunkIndexByIteration::iterator& end);

		/**
		 * Returns an iterator of chunks with an iteration within a given range.
		 */
		ChunkIndexByIteration::iterator getChunksByIterationsRange(int itstart, int itend,
			ChunkIndexByIteration::iterator& end);

		/**
		 * Returns an iterator over all the chunks.
		 * \param[out] end : a reference that will hold the end of the iterator.
		 */
		ChunkIndex::iterator getChunks(ChunkIndex::iterator &end);

		/**
		 * Returns an iterator over all the chunks that correspond to a given
		 * source and iteration.
		 * \param[in] source : source wanted.
		 * \param[in] iteration : iteration wanted.
		 * \param[out] end : a reference that will hold the end of the iterator.
		 */
		ChunkIndex::iterator getChunks(int source, int iteration, ChunkIndex::iterator &end);

		/**
		 * Delete a chunk from the variable, free its memory.
		 */
		void eraseChunk(ChunkIndexBySource::iterator &it);

		/**
		 * Detach a chunk from a variable (don't free the memory).
		 */
		void detachChunk(ChunkIndexBySource::iterator &it);

		/**
		 * Delete a chunk from the variable.
		 */
		void eraseChunk(ChunkIndexByIteration::iterator &it);

		/**
         * Detach a chunk from a variable (don't free the memory).
         */
        void detachChunk(ChunkIndexByIteration::iterator &it);

		/**
		 * Delete all the chunks held by a Variable.
		 */
		void clear();

		/**
		 * Returns a reference to the variabls description.
		 */
		const std::string& getDescription() const { return description; }

		/**
		 * Set the description of the variable.
		 */
		void setDescription(const std::string& desc) { description = desc; }

		/**
		 * Returns the unit of the variable.
		 */
		const std::string& getUnit() const { return unit; }

		/**
		 * Set the unit of the variable.
		 */
		void setUnit(const std::string& u) { unit = u; }

#ifdef __ENABLE_VISIT
		visit_handle getVisItHandle() const;
#endif
};

}

#endif
