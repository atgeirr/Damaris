#include <string>

#include "common/Configuration.hpp"

namespace Damaris {

	Configuration::Configuration(std::string *config, int i)
	{
		id = i;
		configFile = new std::string(*config);
	}

	int Configuration::getCoresPerNode() const 
	{ 
		return 1; 
	}
		
	int Configuration::getNodeID() const
	{
		return id;
	}
	
	std::string* Configuration::getSegmentName() const 
	{ 
		return new std::string("my shared segment");
	}
		
	size_t Configuration::getSegmentSize() const 
	{ 
		return 67108864; 
	}
	
	std::string* Configuration::getMsgQueueName() const
	{ 
		return new std::string("my poke queue");
	}
	
	size_t Configuration::getMsgQueueSize() const
	{ 
		return 100;
	}
	
}

