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
 * \file PyInterpreter.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_PYTHON_H
#define __DAMARIS_PYTHON_H

#include <string>

#include "core/Singleton.hpp"

namespace Damaris {

/**
 * The Python namespace contains everything related to loading Python
 * scripts inside Damaris. The PyInterpreter file contains global functions
 * to initialize Python, execute scripts, etc.
 */
namespace Python {

	class PyInterpreter : public Singleton<PyInterpreter> {
		friend class Singleton<PyInterpreter>;		
	private:
		/**
		 * Constructor, can only be called by the getInstance() method.
		 */
		PyInterpreter();
		/**
		 * Destructor.
		 */
		~PyInterpreter();
		/**
		 * Initialize the Python interpreter.
		 */				
		void initialize();
	
		/**
		 * Finalize the Python interpreter.
		 */	
		void finalize();

	public:
		/**
		 * Executes a given file given as a response to an event sent
		 * by source at a given iteration.
		 */
		void execFile(const std::string &filename, int source, int iteration);
	};
}
}
#endif
