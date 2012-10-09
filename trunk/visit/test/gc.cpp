#include "core/Debug.hpp"
#include "core/VariableManager.hpp"

using namespace Damaris;

extern "C" {

void internal_gc(const std::string& event, int32_t step, int32_t src)
{
	int x = 0;
	Variable* v = VariableManager::Search("life/cells");
	if(v != NULL) {
		ChunkIndexByIteration::iterator end;
		ChunkIndexByIteration::iterator c = v->getChunksByIteration(step, end);
		while(c != end) {
			v->DetachChunk(c->get());
			x++;
			c++;
		}
	}
	INFO("successfully deleted " << x << " chunks");
}

}
