#include "core/Debug.hpp"
#include "core/VariableManager.hpp"

using namespace Damaris;

struct iteration_eraser {

	static Variable* var;
	static int iteration;

	static bool condition(Chunk* c) {
		return c->GetIteration() == iteration;
	}
	
	static void erase(Variable::iterator& it, const Variable::iterator &end, Chunk *c) {
		it = var->DetachChunk(it);
	}
};

extern "C" {

void internal_gc(const std::string& event, int32_t step, int32_t src)
{
//	int x = 0;
	Variable* v = VariableManager::Search("life/cells");
	if(v != NULL) {
		//ChunkIndexByIteration::iterator end;
		//ChunkIndexByIteration::iterator c = v->getChunksByIteration(step, end);
		//while(c != end) {
		//	v->DetachChunk(c->get());
		//	x++;
		//	c++;
		//}
		iteration_eraser::var = v;
		iteration_eraser::iteration = step;
		v->ForEach(iteration_eraser::erase,
				iteration_eraser::condition);
	}
//	INFO("successfully deleted " << x << " chunks");
}

}
