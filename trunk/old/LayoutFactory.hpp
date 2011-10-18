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

#ifndef DAMARIS_LAYOUTFACTORY_H
#define DAMARIS_LAYOUTFACTORY_H
/**
 * The LayoutFactory serializes and unserializes layouts so
 * it can be sent through messages.
 */	
namespace Damaris {
	
class LayoutFactory {
	private:
		
	public:
		/**
		 * \brief Serializes a layout into a buffer of int64_t.
		 * \param[in] layout : Layout to be serialized.
		 * \param[out] buffer : Buffer to hold the result.
		 * return 0.
		 */	
		static int serialize(const Layout* layout, int64_t* buffer)
		{
			buffer[0] = (int64_t)(layout->getType());
			buffer[1] = (int64_t)(layout->getDimensions());
			
			int d = layout->getDimensions();
			for(int i = 0; i < d; i++)
			{
				buffer[2*i+2] = layout->getStartIndex(i);
				buffer[2*i+1+2] = layout->getEndIndex(i);
			}
			return 0;
		}
		
		/**
		 * \brief Unserializes a Layout.
		 * \param[in] buffer : Buffer holding the serialized Layout.
		 * \return A pointer to the unserialized Layout.
		 */
		static Layout* unserialize(const int64_t* buffer) 
		{
			Layout* layout = NULL;
			Types::basic_type_e type = (Types::basic_type_e)(buffer[0]);
			int32_t dimensions = (int32_t)(buffer[1]);
			
			std::vector<int64_t> extents(2*dimensions,0);
			for(int i = 0; i < dimensions*2; i++) 
			{
				extents[i] = buffer[i+2];
			}
			
			layout = new Layout(type,dimensions,extents);
			return layout;
		}
};
}

#endif
