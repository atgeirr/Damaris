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
#include <iostream>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "common/Debug.hpp"
#include "common/ConfigHandler.hpp"
#include "common/Configuration.hpp"

namespace Damaris {
	
	Configuration* Configuration::config = NULL;	

	Configuration::Configuration(std::string *cfgFile, int i)
	{
		id = i;
		configFile = new std::string(*cfgFile);
		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);
		
		ConfigHandler* handler = new ConfigHandler(this);
		parser->setContentHandler(handler);
		parser->setErrorHandler(handler);

		try {
			parser->parse(cfgFile->c_str());
		} catch (const xercesc::XMLException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			ERROR("While parsing XML configuration: " << message);
			xercesc::XMLString::release(&message);
			exit(-1);
		} catch (const xercesc::SAXParseException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			ERROR("While parsing XML configuration: " << message);
			xercesc::XMLString::release(&message);
			exit(-1);
		} catch (...) {
			ERROR("Unexpected Exception while parsing XML configuration.");
			exit(-1);
		}
		delete parser;
		delete handler;
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

