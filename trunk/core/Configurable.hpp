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
 * \file Configurable.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_CONFIGURABLE_H
#define __DAMARIS_CONFIGURABLE_H

namespace Damaris {

/**
 * Configurable is an abstract class for any object that is
 * build uppon a model (instance of ModelClass) and keeps a
 * pointer over this model.
 */
template<typename ModelClass>
	class Configurable {
	
		protected:
			const ModelClass& model; /*!< reference to the base model. */

		public:
			/**
			 * The constructor takes a model as a parameter.
			 */
			Configurable(const ModelClass& mdl) : model(mdl) {	}
	};
}

#endif
