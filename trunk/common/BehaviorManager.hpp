#ifndef __DAMARIS_BEHAVIOR_H
#define __DAMARIS_BEHAVIOR_H

#include <map>
#include <string>

#include "common/MetadataManager.hpp"
#include "common/Reaction.hpp"

namespace Damaris {

class BehaviorManager {
	private:
		std::map<std::string,Reaction*> reactions;
		MetadataManager *metadataManager;
	public:
		BehaviorManager(MetadataManager* mm);
		void reactToPoke(std::string* poke, int32_t iteration, int32_t sourceID);
};

}

#endif
