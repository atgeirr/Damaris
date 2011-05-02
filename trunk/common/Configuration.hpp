/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#ifndef __DAMARIS_CONFIG_H
#define __DAMARIS_CONFIG_H

#include <string>

namespace Damaris {

	class Configuration {
		
	private:
		static Configuration* config;

		std::string* configFile;
		int id;
		std::string* simulationName;
		int coresPerNode;
		std::string* segmentName;
		size_t segmentSize;
		std::string* msgQueueName;
		size_t msgQueueSize;
	public:
		Configuration(std::string* conf, int i);
		
		int getID() {
			return config->getNodeID();
		}

		std::string* getFileName() { return configFile; }
		
		std::string* getSimulationName() { return simulationName; }
		void setSimulationName(char* name) { simulationName = new std::string(name); }

		int getCoresPerNode() const { return coresPerNode; }
		void setCoresPerNode(int cpn) { coresPerNode = cpn; }

		int getNodeID() const;

		std::string* getSegmentName() const { return segmentName; }
		void setSegmentName(char* name) { segmentName = new std::string(name); }

		size_t getSegmentSize() const { return segmentSize; }
		void setSegmentSize(int s) { segmentSize = (size_t)s; }

		std::string* getMsgQueueName() const { return msgQueueName; }
		void setMsgQueueName(char* name) { msgQueueName = new std::string(name); }

		size_t getMsgQueueSize() const { return msgQueueSize; }
		void setMsgQueueSize(int s) { msgQueueSize = s; }
	};

}

#endif
