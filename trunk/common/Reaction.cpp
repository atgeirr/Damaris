#include "common/Reaction.hpp"

namespace Damaris {
	
	Reaction::Reaction()
	{
		function = NULL;
	}
	
	Reaction::Reaction(void(*fptr)(const std::string*, int32_t, int32_t, MetadataManager*))
	{
		function = fptr;
	}
	
	Reaction::~Reaction()
	{}
	
	void Reaction::operator()(const std::string* event, 
							  int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(function != NULL)
			(*function)(event,iteration,sourceID,mm);
	}
	
	void Reaction::call(const std::string* event,
						int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(function != NULL)
			(*function)(event,iteration,sourceID,mm);
	}
	
}