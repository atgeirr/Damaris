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
 * \file PyChunk.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_PYTHON_CHUNK_H
#define __DAMARIS_PYTHON_CHUNK_H

#include <boost/python.hpp>
#include "common/Chunk.hpp"

namespace Damaris {

namespace Python {

namespace bp = boost::python;
	
	/**
	 * The PyChunk class is a decorator class that
	 * exposes a Chunk to a Python environment.
	 */
	class PyChunk {
		friend class PyVariable;
	private:
		Chunk* inner; /*!< Pointer to the inner Chunk object. */

		/**
		 * \brief Constructor.
		 * This constructor throws an error_already_set exception
		 * if the passer Chunk pointer is NULL.
		 * \param[in] c : pointer to the Chunk object to wrap.
		 */
		PyChunk(Chunk* c);

	public:
		/**
		 * \brief Constructor.
		 * This default constructor throws an error_already_set
		 * exception to prevent a PyChunk to be instanciated from
		 * the Python side (only the PyVariable can properly instanciate
		 * a PyChunk by providing a valide inner Chunk to wrap).
		 */
		PyChunk();

		/**
		 * \brief Get the iteration number of the chunk (call Chunk::getIteration).
		 */
		int iteration() const;

		/**
		 * \brief Get the source of the chunk (call Chunk::getSource)
		 */
		int source() const;

		/**
		 * \brief Get the type (as a string) of the chunk.
		 */
		const std::string& type() const;

		/**
		 * \brief Get the start indices of the chunk as a Python list.
		 */
		bp::list lower_bounds() const;
	
		/**
		 * \brief Get the end indices of the chunk as a Python list.
		 */	
		bp::list upper_bounds() const;

		/**
		 * \brief Gets the data wrapped as a NumPy array.
		 * None is returned if the data is NULL.
		 */
		bp::object data() const;

		/**
		 * \brief Removes the data if it's in local memory.
		 */
		void remove();
	};
}

}
#endif
