#ifndef __DAMARIS_LAYOUT_H
#define __DAMARIS_LAYOUT_H

#include <valarray>
#include <vector>
#include <stdint.h>

/**
 * Layouts are objects that describe the representation of
 * data in memory.
 */
namespace Damaris {
	
	enum basic_type_e {
		SHORT  = 1,	// short int, integer*2
		INT    = 2,	// int, integer*4
		LONG   = 3,	// long int, integer*8
		FLOAT  = 4,	// float, real
		DOUBLE = 5,	// double, real*8
		CHAR   = 6,	// char, character
		
		UNDEFINED_TYPE = 0  // don't know
	}; // enum basic_type_e
	
	/* this function gives the size (in bytes) of each type */
	int basicTypeSize(basic_type_e t);
	
class Layout {
		
	private:
		basic_type_e type; /* type of the data */
		int32_t dimensions; /* number of dimensions */
		int64_t* startIndex; /* start index along each dimension */
		int64_t* endIndex; /* end index along each dimencion */
		
	public:
		/* initialize a layout from the data type, the dimensions d and the vector of extents */
		Layout(basic_type_e t, int32_t d, std::vector<int64_t> extents);
		/* idem but the extents are unknow yet */
		Layout(basic_type_e t, int32_t d);
		/* destructor */
		~Layout();
		
		/* returns the size of a buffer that would hold the data */
		size_t  getRequiredMemoryLength() const;
		/* returns the type of the data */
		basic_type_e getType() const;
		/* returns the number of dimensions */
		int32_t getDimensions() const;
		/* returns the starting index along a particular dimension */
		int64_t getStartIndex(int dim) const;
		/* returns the ending index along a particular dimension */
		int64_t getEndIndex(int dim) const;
		/* return the extent (start-end+1) along a given dimension */
		int64_t getExtentAlongDimension(int dim) const;
		
}; // class Layout
	
} // namespace Damaris

#endif
