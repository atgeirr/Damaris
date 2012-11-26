#include "core/Debug.hpp"
#include "core/VariableManager.hpp"

using namespace Damaris;

struct iteration_eraser {

	static Variable* var;
	static int iteration;

	static bool condition(Chunk* c) {
		return c->GetIteration() == iteration;
	}
	
	static void erase(Variable::iterator& it, const Variable::iterator& /*end*/, Chunk* /*c*/) {
		it = var->DetachChunk(it);
	}
};

Variable* iteration_eraser::var = NULL;
int iteration_eraser::iteration = 0;

extern "C" {

void clean(const std::string& /*event*/, int32_t step, int32_t /*src*/)
{
	Variable* v = VariableManager::Search("space");
	if(v != NULL) {
		iteration_eraser::var = v;
		iteration_eraser::iteration = step-2;
		v->ForEach(iteration_eraser::erase,
				iteration_eraser::condition);
	}
}

}
