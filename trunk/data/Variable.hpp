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

		std::string name; 	/*!< Name of the variable. */
		int id;				/*!< id of the variable, given by the VariableManager. */
	
		/**
		 * \brief Constructor. 
		 * Creates a Variable record given a model (from XML), a name and a layout.
		 * The constructor is private, use New to create an instance.
		 */
		Variable(const Model::Variable& v, const std::string& name, Layout* l);

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
		 * Returns the id of the variable, as given by the VariableManager.
		 */
		int getID() const { return id; }

		/**
		 * Attach a new chunk to the variable.
		 */
		void attachChunk(Chunk* chunk);

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
		 * Detach a chunk from a variable. Free its memory if the process owns the chunk.
		 * To prevent from loosing data, don't forget to unset the chunk's ownership before
		 * detaching the chunk.
		 */
		void detachChunk(Chunk* c);

		/**
		 * Delete all the chunks held by a Variable.
		 */
		void clear();

		/**
		 * Returns the variables description.
		 */
		std::string getDescription() const { return (std::string)model; }

		/**
		 * Returns the unit of the variable.
		 */
		std::string getUnit() const { return model.unit(); }

#ifdef __ENABLE_VISIT
		/**
		 * Fills VisIt's metadata handle with information related to the variable.
		 */
		bool exposeVisItMetaData(visit_handle md);

		/**
		 * Fills VisIt's data handle with the proper data.
		 */
		bool exposeVisItData(visit_handle *md, int source, int iteration);

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
		 * Creates an instance of Variable if the provided model is consistant.
		 */
		static Variable* New(const Model::Variable& mdl, const std::string &name);
};

}

#endif
