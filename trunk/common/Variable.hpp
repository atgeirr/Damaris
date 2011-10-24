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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <stdint.h>
#include <list>
#include <string>
#include "common/ChunkSet.hpp"
#include "common/Layout.hpp"

namespace Damaris {

class MetadataManager;

/**
 * The Variable object is used for describing a variable within
 * a metadata structure. It holds an index of chunks and additional
 * informations. A Variable record is identified by a name and an ID.
 * A Variable cannot be created in a vacuum, only the MetadataManager
 * has the permission to create instance of them.
 */
class Variable {
	friend class MetadataManager;
	
	private:	
		std::string name;	/*!< Name of the variable. */
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
				ChunkIndexBySource::iterator &end);

		/**
		 * Returns the list of chunks with a specified iteration.
		 */
		ChunkIndexByIteration::iterator getChunksByIteration(int iteration, 
				ChunkIndexByIteration::iterator &end);

		/**
		 * Delete a chunk from the variable (will not free the memory
		 * associated to the chunk).
		 */
		void eraseChunk(ChunkIndexBySource::iterator &it);
		void eraseChunk(ChunkIndexByIteration::iterator &it);
};

}

#endif
