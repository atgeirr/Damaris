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
 * \file PyVariable.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_PYTHON_VARIABLE_H
#define __DAMARIS_PYTHON_VARIABLE_H

#include <boost/python.hpp>
#include "common/Variable.hpp"
#include "scripts/python/PyChunk.hpp"
#include "scripts/python/PyLayout.hpp"


namespace Damaris {

namespace Python {

namespace bp = boost::python;

	/**
	 * PyVariable is a decorator that wraps a Variable object 
	 * into an object that Python can manipulate.
	 */	
	class PyVariable {
	private:
		Variable* inner; /*!< wrapped Variable object. */

	public:
		/**
		 * Default constructor, throws an error_already_set exception
		 * to prevent PyVariable from being instanciated from Python.
		 */
		PyVariable();

		/**
		 * \brief Constructor.
		 * Takes a pointer to a Variable to wrap. Throws error_already_set
		 * if the pointer is NULL.
		 */
		PyVariable(Variable* v);

		/**
		 * Build a list of PyChunks from a hash associating
		 * "source" : source id
		 * "iteration" : iteration number
		 * If one (or both) the hash fields are not set, 
		 * returns all entries satifying the conditions that are set.
		 */
		bp::list select(const bp::dict& args) const;

		/**
		 * Gets all chunks (equivalent to PyVariable::select with and empty hash).
		 */
		bp::list chunks() const;

		/**
		 * Return the name of the variable (not the full path).
		 */
		std::string name() const;
	
		/**
		 * Return the full path (groups/sugroups/.../varname).
		 */
		const std::string& fullname() const;

		/**
		 * Return a PyLayout wrapping the wrapped Variable's Layout.
		 */
		PyLayout layout() const;

		/**
		 * Remove a chunk from the variable, free the memory of the chunk
		 */
		bool remove(PyChunk& pc);

		void clear();
	};
}
}
#endif
