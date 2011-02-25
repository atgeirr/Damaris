#ifndef __DAMARIS_CONFIG_H
#define __DAMARIS_CONFIG_H

namespace Damaris {
	
	class Configuration {
	public:
		int    getCoresPerNode() const 
			{ return 1; }
		std::string*  getSegmentName() const 
		{ return new std::string("my shared segment");}
		size_t getSegmentSize() const 
			{ return 67108864; }
		std::string*  getMsgQueueName() const
		{ return new std::string("my poke queue"); }
		size_t getMsgQueueSize() const
			{ return 100;}
	};
	
}

#endif
