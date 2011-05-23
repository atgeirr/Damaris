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

#include <map>
#include <string>

#include "common/Parameter.hpp"

namespace Damaris {
	/** 
	 * The class Configuration holds all informations extrated from
	 * both the command lines (for the server) and the external configuration
	 * file. It is independant of the format of this configuration file
	 * (which is loaded through the ConfigHandler object)
	 */
	class Configuration {
		
	private:
		/**
		 * for information, we keep the name of the external configuration file
		 */
		std::string* configFile;
		/**
		 * Name of the simulation we are running
		 */
		std::string* simulationName;
		/**
		 * number of cores per node
		 */
		int coresPerNode;
		/**
		 * name of the shared buffer
		 */
		std::string* segmentName;
		/**
		 * size of the buffer (in bytes)
		 */
		size_t segmentSize;
		/**
		 * name of the shared message queue
		 */
		std::string* msgQueueName;
		/**
		 * maximum number of messages in the queue
		 */
		size_t msgQueueSize;
		/**
		 * list of parameters
		 */
		std::map<std::string,Parameter>* parameters;
	public:
		Configuration(std::string* configName);
		~Configuration();

		std::string* getFileName() { return configFile; }
		
		std::string* getSimulationName() { return simulationName; }
		void setSimulationName(char* name) { simulationName = new std::string(name); }

		int getCoresPerNode() const { return coresPerNode; }
		void setCoresPerNode(int cpn) { coresPerNode = cpn; }

		std::string* getSegmentName() const { return segmentName; }
		void setSegmentName(char* name) { segmentName = new std::string(name); }

		size_t getSegmentSize() const { return segmentSize; }
		void setSegmentSize(int s) { segmentSize = (size_t)s; }

		std::string* getMsgQueueName() const { return msgQueueName; }
		void setMsgQueueName(char* name) { msgQueueName = new std::string(name); }

		size_t getMsgQueueSize() const { return msgQueueSize; }
		void setMsgQueueSize(int s) { msgQueueSize = s; }

		
		void setParameter(char* name, char* type, char* value);
		
		void setVariable(char* name, char* layoutName);
	};

}

#endif
