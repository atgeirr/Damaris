#include "common/MetadataManager.hpp"

using namespace std;

namespace Damaris {
	
	Variable* MetadataManager::get(std::string* name, int32_t iteration, int32_t sourceID)
	{
		return NULL;
	}
	
	void MetadataManager::put(Variable* v)
	{
		vars.push_back(v);
	}
	
	void MetadataManager::put(std::string* name, int32_t iteration, int32_t sourceID, Layout* l, void* data)
	{
		Variable* v = new Variable(name,iteration,sourceID,l,data);
		vars.push_back(v);
	}
	
	void MetadataManager::remove(std::string* name, int32_t iteration, int32_t sourceID)
	{
		Variable* v = get(name,iteration,sourceID);
		vars.remove(v);
	}
	
	MetadataManager::MetadataManager(managed_shared_memory* s)
	{
		segment = s;
	}
	
	std::list<Variable*>* MetadataManager::getAllVariables()
	{
		return &vars;
	}
	
	MetadataManager::~MetadataManager()
	{
		vars.clear();
	}
	
}
