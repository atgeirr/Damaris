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
#ifndef __DAMARIS_ENVIRONMENT_H
#define __DAMARIS_ENVIRONMENT_H


namespace Damaris {
/** 
 * The class Environment holds all local informations
 * such as the id of the enclosing process.
 */
class Environment {
		
	private:
		int id; /*!< ID of the server. */
	public:
		
		/**
		 * \brief Constructor.
	  	 */
		Environment();

		/**
		 * \brief Constructor taking an ID. 
		 * \param[in] i : id of the server.
		 */
		Environment(int i);

		/**
		 * \brief Get the ID of the server.
		 * \return ID of the server.
		 */
		int getID() { return id; }
		
		/**
		 * \brief Set the ID of the server.
		 * \param[in] i : new ID.
		 */
		void setID(int i) { id = i; }

};

}

#endif
