#ifndef __DAMARIS_REACTION_H
#define __DAMARIS_REACTION_H

#include <stdint.h>
#include <string>

#include "common/MetadataManager.hpp"

namespace Damaris {

class Reaction {
	private:
		void (*function)(const std::string*,int32_t,int32_t,MetadataManager*);
	public:
		Reaction();
		Reaction(void(*fptr)(const std::string*, int32_t, int32_t, MetadataManager*));
		~Reaction();
		
		void operator()(const std::string* event, 
				int32_t iteration, int32_t sourceID, MetadataManager* mm);
		void call(const std::string* event,
			  int32_t iteration, int32_t sourceID, MetadataManager* mm);
};

}

#endif
