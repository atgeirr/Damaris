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

#include <string>
#include "common/Configuration.hpp"

namespace Damaris {
	
	Configuration* Configuration::config = NULL;	

	Configuration::Configuration(std::string *cfgFile, int i)
	{
		id = i;
		configFile = new std::string(*cfgFile);
		Damaris::Configuration::config = this;
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

