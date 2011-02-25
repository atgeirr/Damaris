#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <stdint.h>
#include <string>
#include "common/Layout.hpp"

namespace Damaris {
	
	class Variable {
	private:
		std::string* name;   // name of the variable
		int64_t iteration;   // iteration of publication
		int32_t sourceID; // source that published this variable
		Layout* layout;     // layout of the data
		void* data;     // pointer on the data
	
	public:
		Variable(std::string* vname, int32_t it, int32_t src, Layout* l, void* d);
		std::string* getName() const;
		int32_t getIteration() const;
		int32_t getSource() const;
		Layout* getLayout() const;
		void* getDataAddress() const;
		bool compare(const Variable &v) const;
		void print() const; 
	};
	
}

#endif
