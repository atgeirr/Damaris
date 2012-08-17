
#include "core/ActionManager.hpp"
#include "event/BoundAction.hpp"
#include "client/Definition.hpp"

extern "C" {

using namespace Damaris;

	int DC_bind_function(const char* event, BoundAction::fun_t f) {
		BoundAction* a = BoundAction::New(f,std::string(event));
		if(a == NULL) return -1;
		if(not ActionManager::Add<BoundAction>(a)) {
			delete a;
			return -1;
		}
		return 0;
	}

}
