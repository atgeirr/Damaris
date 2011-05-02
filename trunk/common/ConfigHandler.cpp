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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#include "common/Debug.hpp"
#include "common/ConfigHandler.hpp"

using namespace xercesc;

namespace Damaris {

	ConfigHandler::ConfigHandler(Configuration* c)
	{
		config = c;

		try
		{
			XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
		} catch( XMLException& e ) {
			char* message = XMLString::transcode(e.getMessage());
      			ERROR("XML toolkit initialization error: " << message);
      			XMLString::release(&message);
			exit(-1);
		}

		configFileParser = new xercesc::XercesDOMParser();

		TAG_simulation 		= XMLString::transcode("simulation");
		TAG_nodes 		= XMLString::transcode("nodes");
		TAG_nodes_cores 	= XMLString::transcode("cores");
		TAG_nodes_buffer 	= XMLString::transcode("buffer");
 		TAG_nodes_queue 	= XMLString::transcode("queue");
		
		ATTR_name 		= XMLString::transcode("name");
		ATTR_count 		= XMLString::transcode("count");
		ATTR_size 		= XMLString::transcode("size");
	}

	ConfigHandler::~ConfigHandler()
	{
		delete configFileParser;

		try {
			XMLString::release(&TAG_simulation);
			XMLString::release(&TAG_nodes);
			XMLString::release(&TAG_nodes_cores);
			XMLString::release(&TAG_nodes_buffer);
			XMLString::release(&TAG_nodes_queue);
			
			XMLString::release(&ATTR_name);
			XMLString::release(&ATTR_count);
			XMLString::release(&ATTR_size);
		} catch( ... ) {
			ERROR("Error while releasing Xerces-C resources");
		}

		try {
			XMLPlatformUtils::Terminate();  // Terminate after release of memory
		} catch( xercesc::XMLException& e ) {
			char* message = xercesc::XMLString::transcode( e.getMessage() );
			ERROR("XML toolkit teardown error: " << message);
      			XMLString::release( &message );
   		}
	}

	void ConfigHandler::readConfigFile(std::string* configFile)
	{
   		// Test to see if the file is ok.
		struct stat fileStatus;
		int iretStat = stat(configFile->c_str(), &fileStatus);
		if( iretStat == ENOENT )
			throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
		else if( iretStat == ENOTDIR )
			throw ( std::runtime_error("A component of the path is not a directory."));
		else if( iretStat == ELOOP )
			throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
		else if( iretStat == EACCES )
			throw ( std::runtime_error("Permission denied."));
		else if( iretStat == ENAMETOOLONG )
			throw ( std::runtime_error("File can not be read"));

		// Configure DOM parser.
		configFileParser->setValidationScheme( XercesDOMParser::Val_Never );
		configFileParser->setDoNamespaces( false );
		configFileParser->setDoSchema( false );
		configFileParser->setLoadExternalDTD( false );

		try {
			configFileParser->parse( configFile->c_str() );

			// no need to free this pointer - owned by the parent parser object
			DOMDocument* xmlDoc = configFileParser->getDocument();
			// Get the top-level element: Name is "simulation".
			      
			DOMElement* elementSimulation = xmlDoc->getDocumentElement();
			if( !elementSimulation ) 
				throw(std::runtime_error( "empty XML document" ));

			// check that this element is named "simulation"
			if(!XMLString::equals(elementSimulation->getTagName(), TAG_simulation))
				throw(std::runtime_error("XML document does not start with a \"simulation\" element"));

			const XMLCh* xmlch_attr = elementSimulation->getAttribute(ATTR_name);
			char* char_attr = XMLString::transcode(xmlch_attr);
                        config->setSimulationName(char_attr);
			INFO("Parsing configuration file for simulation " << char_attr);
			XMLString::release(&char_attr);

			// Look one level nested within "simulation". (child of simulation)
			DOMNodeList* children = elementSimulation->getChildNodes();
			const  XMLSize_t nodeCount = children->getLength();
			// For all nodes, children of "simulation" in the XML tree.
			for( XMLSize_t i = 0; i < nodeCount; ++i)
			{
				DOMNode* currentNode = children->item(i);
				if( currentNode->getNodeType() &&  // true is not NULL
				    currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
				{
					// Found node which is an Element. Re-cast node as element
					DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
					if(XMLString::equals(currentElement->getTagName(), TAG_nodes))
						readNodesConfig(currentElement);
				}
			}

		} catch( xercesc::XMLException& e ) {
			char* message = xercesc::XMLString::transcode( e.getMessage() );
			ERROR("Error parsing file: " << message);
			XMLString::release( &message );
		} catch( std::runtime_error& e) {
			ERROR("Runtime error while parsing XML file: " << e.what());
		}
	}

	void ConfigHandler::readNodesConfig(DOMElement* elem) throw ()
	{
		INFO("Parsing internal configuration of nodes");
		// elem is a <nodes> element, it can have the following childs
		// <cores>, <buffer>, <queue>
		// iterates on childs
		DOMNodeList* children = elem->getChildNodes();
		const  XMLSize_t nodeCount = children->getLength();
		
		char* char_attr;
		// For all nodes, children of "nodes" in the XML tree.
		for(XMLSize_t i = 0; i < nodeCount; ++i )
		{
			DOMNode* currentNode = children->item(i);
			
			if( currentNode->getNodeType() &&
			    currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
				// <cores> parsing
				if( XMLString::equals(currentElement->getTagName(), TAG_nodes_cores))
				{
					const XMLCh* xmlch_attr = currentElement->getAttribute(ATTR_count);
					char_attr = XMLString::transcode(xmlch_attr);
					
					config->setCoresPerNode(atoi(char_attr));
					INFO("Using " << atoi(char_attr) << " cores per node");

					XMLString::release(&char_attr);
					continue;
				} else
				// <buffer> parsing
				if( XMLString::equals(currentElement->getTagName(), TAG_nodes_buffer))
				{
					const XMLCh* xmlch_name = currentElement->getAttribute(ATTR_name);
					const XMLCh* xmlch_size = currentElement->getAttribute(ATTR_size);

					char_attr = XMLString::transcode(xmlch_name);
					config->setSegmentName(char_attr);
					INFO("Buffer name is " << char_attr);
					XMLString::release(&char_attr);
					
					char_attr = XMLString::transcode(xmlch_size);
					config->setSegmentSize(atoi(char_attr));
					INFO("Buffer size is " << char_attr);
					XMLString::release(&char_attr);
					continue;
				} else
				// <queue> parsing
				if( XMLString::equals(currentElement->getTagName(), TAG_nodes_queue))
				{
					const XMLCh* xmlch_name = currentElement->getAttribute(ATTR_name);
					const XMLCh* xmlch_size = currentElement->getAttribute(ATTR_size);
					
					char_attr = XMLString::transcode(xmlch_name);
					config->setMsgQueueName(char_attr);
					INFO("Message queue name is " << char_attr);
					XMLString::release(&char_attr);

					char_attr = XMLString::transcode(xmlch_size);
					config->setMsgQueueSize(atoi(char_attr));
					INFO("Message queue size is " << char_attr);
					XMLString::release(&char_attr);
					continue;
				}
			}
		}
	}
}
