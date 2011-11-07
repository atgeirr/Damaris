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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_PYTHON_LAYOUT_H
#define __DAMARIS_PYTHON_LAYOUT_H

#include <boost/python.hpp>

#include "common/Layout.hpp"

namespace Damaris {

namespace bp = boost::python;
	
	class PyLayout {
	private:
		Layout* inner;

	public:
		PyLayout();

		PyLayout(Layout* l);

		const std::string& name() const;
	
		const std::string& type() const;
	
		bp::list extents() const;
	};
}

#endif
