/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef __DAMARIS_CONFIGURABLE_H
#define __DAMARIS_CONFIGURABLE_H

namespace damaris {

/**
 * Configurable is an abstract class for any object that is built upon a model 
 * (instance of M class) and keeps a reference over this model.
 */
template<typename M>
class Configurable {
	
	private:
	
	const M& model_; /*!< base model. */

	public:
		
	/**
	 * The constructor takes a model as a parameter.
	 * 
	 * \param[in] mdl : model from which the object is configured.
	 */
	Configurable(const M& mdl) : model_(mdl) {}
		
	/**
	 * Returns the model.
	 */
	const M& GetModel() const {
		return model_;
	}
};

}

#endif
