#ifndef __DAMARIS_CONFIG_H
#define __DAMARIS_CONFIG_H

#include <string>

namespace Damaris {

	class Configuration {
		
	private:
		static Configuration* config;
		std::string* configFile;
		int id;
	public:
		Configuration(std::string* conf, int i);
		
		static int getID() {
			return config->getNodeID();
		}
		int getCoresPerNode() const; 
		int getNodeID() const;
		std::string*  getSegmentName() const; 
		size_t getSegmentSize() const;
		std::string*  getMsgQueueName() const;
		size_t getMsgQueueSize() const;
	};

}

#endif
