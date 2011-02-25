#include <cstring>
#include "common/Util.hpp"
#include "common/Layout.hpp"

namespace Damaris {
	
	int basicTypeSize(basic_type_e t)
	{
		switch(t) {
			case SHORT :
				return sizeof(short int);
			case INT :
				return sizeof(int);
			case LONG :
				return sizeof(long int);
			case FLOAT :
				return sizeof(float);
			case DOUBLE :
				return sizeof(double);
			case CHAR :
				return sizeof(char);
			case UNDEFINED_TYPE :
				return 0;
		}
		return 0;
	}
	
	Layout::Layout(basic_type_e t, int32_t d, std::vector<int64_t> extents)
	{
		type = t;
		dimensions = d;
		if((int)extents.size() != (2*d)) {
			LOG("Error in layout initialization\n")
		}
		if(dimensions > 0){
			startIndex = new int64_t[dimensions];
			endIndex = new int64_t[dimensions];
			for(int i = 0; (i < dimensions) && (2*i+1 < (int)extents.size()); i++)
			{
				startIndex[i] = extents[2*i];
				endIndex[i] = extents[2*i+1];
			}
		} else {
			startIndex = NULL;
			endIndex = NULL;
		}
	}
	
	Layout::Layout(basic_type_e t, int32_t d)
	{
		type = t;
		dimensions = d;
		if(d != 0) {
			LOG("Error in layout initialization\n")
		}
		startIndex = NULL;
		endIndex = NULL;
	}
	
	Layout::~Layout()
	{
		if(startIndex) delete startIndex;
		if(endIndex) delete endIndex;
	}
	
	size_t  Layout::getRequiredMemoryLength() const
	{
		size_t result = 1;
		result *= basicTypeSize(type);
		for(int i = 0; i < dimensions; i++)
			result *= (endIndex[i]-startIndex[i]+1);
		return result;
	}
	
	basic_type_e Layout::getType() const
	{
		return type;
	}
	
	int32_t Layout::getDimensions() const
	{
		return dimensions;
	}
	
	int64_t Layout::getStartIndex(int dim) const
	{
		return startIndex[dim];
	}
	
	int64_t Layout::getEndIndex(int dim) const
	{
		return endIndex[dim];
	}

	int64_t Layout::getExtentAlongDimension(int dim) const
	{
		if(dim < dimensions)
			return (endIndex[dim]-startIndex[dim]+1);
		else
			return 0;
	}	
}
