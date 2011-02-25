#ifndef __DAMARIS_LAYOUT_H
#define __DAMARIS_LAYOUT_H

#include <valarray>
#include <vector>
#include <stdint.h>

namespace Damaris {
	
	enum basic_type_e {
		SHORT  = 1,  // short int
		INT    = 2,  // int
		LONG   = 3,  // long int
		FLOAT  = 4,  // float
		DOUBLE = 5, // double
		CHAR   = 6,   // char
		
		UNDEFINED_TYPE = 0  // don't know
	}; // enum basic_type_e
	
	int basicTypeSize(basic_type_e t);
	
	class Layout {
		
	private:
		basic_type_e type;
		int32_t dimensions;
		int64_t* startIndex;
		int64_t* endIndex;
		
	public:
		Layout(basic_type_e t, int32_t d, std::vector<int64_t> extents);
		Layout(basic_type_e t, int32_t d);
		~Layout();
		
		size_t  getRequiredMemoryLength() const;
		basic_type_e getType() const;
		int32_t getDimensions() const;
		int64_t getStartIndex(int dim) const;
		int64_t getEndIndex(int dim) const;
		int64_t getExtentAlongDimension(int dim) const;
		
	}; // class Layout
	
} // namespace Damaris

#endif
