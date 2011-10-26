#include <stdio.h>
#include "common/ChunkSet.hpp"
#include "common/Variable.hpp"
#include "common/MetadataManager.hpp"

extern "C" {

void draw_image(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{
	static int calls;

	calls += 1;
	if(calls != 4) return;
	calls = 0;

	Damaris::Variable* v = db->getVariable("images/julia");
	if(v != NULL) {
		Damaris::ChunkIndexByIteration::iterator end;
		Damaris::ChunkIndexByIteration::iterator it = v->getChunksByIteration(step,end);
		v->eraseChunk(it);
	}
}

}
