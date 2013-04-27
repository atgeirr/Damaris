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
 * \file Position.hpp
 * \date May 2013
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_POSITION_H
#define __DAMARIS_POSITION_H

#include <stdlib.h>

namespace Damaris {
	
	/**
	 * This class aim at representing a position in a N-dimentional
	 * space (integer positions). It is used to locate a chuck within
	 * a bigger space.
	 */
	class Position {
		private:	
		std::vector<int64_t> offsets;
	
		public:
		Position() {}

		Position(std::vector<int64_t> d)
		: offsets(d.size())
		{
			for(unsigned int i=0; i < d.size(); i++) {
				offsets[i] = d[i];
			}
		}
		
		int64_t GetOffset(unsigned int dimension) const
		{
			if(dimension < offsets.size()) {
				return offsets[dimension];
			} else {
				return 0;
			}
		}

		void SetOffset(unsigned int dimension, int64_t offset)
		{
			if(dimension >= offsets.size()) {
				offsets.resize(dimension+1);
			}
			offsets[dimension] = offset;
		}
	}; // class Position

} // namespace Damaris

#endif
