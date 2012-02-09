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
 * \file PyLayout.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_PYTHON_LAYOUT_H
#define __DAMARIS_PYTHON_LAYOUT_H

#include <boost/python.hpp>

#include "common/Layout.hpp"

namespace Damaris {

namespace Python {

namespace bp = boost::python;

	/**
	 * PyLayout is a decorator class for a Layout object to
	 * be exposed as a Python object.
	 */	
	class PyLayout {
		friend class PyVariable;

	private:
		Layout* inner; /*!< Wrapped Layout object. */

		/**
		 * \brief Constructor.
		 * This constructor can only be called by PyVariable.
		 * It will throw an error_already_set in case the wrapped object is NULL.
		 */
		PyLayout(Layout* inner);
	public:
		/**
		 * \brief Constructor.
		 * This constructor throws a error_already_set exception
		 * to prevent PyLayout from being instanciated from Python.
		 * Only the PyVariable class is allowed to build this object
		 * using the second construyctor.
		 */
		PyLayout();

		/**
		 * \brief Returns the name of the inner Layout.
		 */
		const std::string& name() const;
	
		/**
		 * \brief Return the type of the inner Layout.
		 */
		const std::string& type() const;
	
		/**
		 * \brief Return the extents of the Layout as a Python list.
		 */
		bp::list extents() const;
	};
}

}
#endif
