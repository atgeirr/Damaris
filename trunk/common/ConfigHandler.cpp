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
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#include <boost/algorithm/string.hpp>

#include "common/Debug.hpp"
#include "common/ConfigHandler.hpp"

using namespace xercesc;

namespace Damaris {
	
	/* Constructor, takes a Configuration object */
	ConfigHandler::ConfigHandler(Configuration* c)
	{
		config = c;
		nodeParsed = false;

		/* try initializing the XML utilities */
		try
		{
			XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
		} catch( XMLException& e ) {
			char* message = XMLString::transcode(e.getMessage());
      			ERROR("XML toolkit initialization error: " << message);
      			XMLString::release(&message);
			/* if Xerces-C has not been properly initialized, well there is no
			   point continuing. We can stop here without leaving shared objects
			   opened because shared objects are created after reading the
			   configuration file anyway. */
			exit(-1);
		}

		configFileParser = new xercesc::XercesDOMParser();

		/* initializing all tags */
		TAG_simulation 		= XMLString::transcode("simulation");
		TAG_nodes 		= XMLString::transcode("nodes");
		TAG_nodes_cores 	= XMLString::transcode("cores");
		TAG_nodes_buffer 	= XMLString::transcode("buffer");
 		TAG_nodes_queue 	= XMLString::transcode("queue");
		TAG_data		= XMLString::transcode("data");
		TAG_data_variable	= XMLString::transcode("variable");
		TAG_data_layout		= XMLString::transcode("layout");
		TAG_data_parameter	= XMLString::transcode("parameter");
		TAG_actions		= XMLString::transcode("actions");
		TAG_actions_event	= XMLString::transcode("event");
		
		/* initializing all attributes */
		ATTR_name 		= XMLString::transcode("name");
		ATTR_count 		= XMLString::transcode("count");
		ATTR_size 		= XMLString::transcode("size");
		ATTR_type		= XMLString::transcode("type");
		ATTR_layout		= XMLString::transcode("layout");
		ATTR_dimensions		= XMLString::transcode("dimensions");
		ATTR_language		= XMLString::transcode("language");
		ATTR_value		= XMLString::transcode("value");
		ATTR_action		= XMLString::transcode("action");
		ATTR_using		= XMLString::transcode("using");
	}

	ConfigHandler::~ConfigHandler()
	{
		/* first delete the parser */
		if(configFileParser != NULL)
			delete configFileParser;

		/* now deleting all resources used by tags and attributes */
		try {
			XMLString::release(&TAG_simulation);
			XMLString::release(&TAG_nodes);
			XMLString::release(&TAG_nodes_cores);
			XMLString::release(&TAG_nodes_buffer);
			XMLString::release(&TAG_nodes_queue);
			XMLString::release(&TAG_data);
			XMLString::release(&TAG_data_parameter);
			XMLString::release(&TAG_data_variable);
			XMLString::release(&TAG_data_layout);
			XMLString::release(&TAG_actions);
			XMLString::release(&TAG_actions_event);
			
			XMLString::release(&ATTR_name);
			XMLString::release(&ATTR_count);
			XMLString::release(&ATTR_size);
			XMLString::release(&ATTR_type);
			XMLString::release(&ATTR_layout);
			XMLString::release(&ATTR_dimensions);
			XMLString::release(&ATTR_language);
			XMLString::release(&ATTR_value);
			XMLString::release(&ATTR_action);
			XMLString::release(&ATTR_using);

		} catch( ... ) {
			ERROR("Error while releasing Xerces-C resources");
		}

		/* try terminating all XML-related stuff */
		try {
			XMLPlatformUtils::Terminate();  // Terminate after release of memory
		} catch( xercesc::XMLException& e ) {
			char* message = xercesc::XMLString::transcode( e.getMessage() );
			ERROR("XML toolkit teardown error: " << message);
      			XMLString::release( &message );
   		}
	}

	/* reads a particular configuration file */
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
					
					// Does this element is a <node> ?
					if(XMLString::equals(currentElement->getTagName(), TAG_nodes))
						readNodesConfig(currentElement);
					// Does this element is a <data> ?
					if(XMLString::equals(currentElement->getTagName(), TAG_data))
						readDataConfig(currentElement);
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

	/* Parse configuration within the <node> element 
	   This function can be called only once */
	void ConfigHandler::readNodesConfig(DOMElement* elem) throw ()
	{
		if(nodeParsed)
		{
			WARN("Several <node> items found in configuration, only the first one is considered.");
			return;
		}
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

		nodeParsed = true;
	}

	
	/* Parse configuration within the <data> element  */
	void ConfigHandler::readDataConfig(DOMElement* elem) throw ()
	{
		INFO("Parsing configuration for data");
		// elem is a <data> element, it can have the following childs
		// <variable>, <layout>, <parameter>
		// iterates on childs
		DOMNodeList* children = elem->getChildNodes();
		const  XMLSize_t nodeCount = children->getLength();
		
		// For all nodes, children of "data" in the XML tree.
		for(XMLSize_t i = 0; i < nodeCount; ++i )
		{
			DOMNode* currentNode = children->item(i);
			
			if( currentNode->getNodeType() &&
			    currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
				//  Does the element equals <parameter>
				if( XMLString::equals(currentElement->getTagName(), TAG_data_parameter))
				{
					readParameterInfo(currentElement);
					continue;
				} else
				// Does the element equals <layout>
				if( XMLString::equals(currentElement->getTagName(), TAG_data_layout))
				{
					readLayoutInfo(currentElement);
					continue;
				} else
				// Does the element equals <variable>
				if( XMLString::equals(currentElement->getTagName(), TAG_data_variable))
				{
					readVariableInfo(currentElement);
					continue;
				}
			}
		}
	}

	
	/* Parse configuration within the <actions> element  */
	void ConfigHandler::readActionsConfig(DOMElement* elem) throw ()
	{
		INFO("Parsing configuration for actions");
		// elem is a <actions> element, it can have the following childs
		// <event>
		// iterates on childs
		DOMNodeList* children = elem->getChildNodes();
		const  XMLSize_t nodeCount = children->getLength();
		
		// For all nodes, children of "data" in the XML tree.
		for(XMLSize_t i = 0; i < nodeCount; ++i )
		{
			DOMNode* currentNode = children->item(i);
			
			if( currentNode->getNodeType() &&
			    currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
				//  Does the element equals <event>
				if( XMLString::equals(currentElement->getTagName(), TAG_actions_event))
				{
					readEventInfo(currentElement);
					continue;
				}
			}
		}
	}
	
	/* this function is called when finding a <parameter> tag */
	void ConfigHandler::readParameterInfo(DOMElement* elem) throw()
	{
		/* getting attributes */
		const XMLCh* xmlch_name = elem->getAttribute(ATTR_name);
		const XMLCh* xmlch_type = elem->getAttribute(ATTR_type);
		const XMLCh* xmlch_value = elem->getAttribute(ATTR_value);

		/* converting into char* */
		char* attr_name = XMLString::transcode(xmlch_name);
		char* attr_type = XMLString::transcode(xmlch_type);
		char* attr_value = XMLString::transcode(xmlch_value);

		/* setting parameter in configuration */
		config->setParameter(attr_name, attr_type, attr_value);

		/* releasing memory */
		XMLString::release(&attr_name);
		XMLString::release(&attr_type);
		XMLString::release(&attr_value);
	}
	
	/* this function is called when finding a <variable> tag */
	void ConfigHandler::readVariableInfo(DOMElement* elem) throw()
	{
		/* getting attributes */
		const XMLCh* xmlch_name = elem->getAttribute(ATTR_name);
		const XMLCh* xmlch_layout = elem->getAttribute(ATTR_layout);

		/* converting into char* */
		char* attr_name = XMLString::transcode(xmlch_name);
		char* attr_layout = XMLString::transcode(xmlch_layout);

		/* inserting variable into configuration */
		config->setVariable(attr_name,attr_layout);
		
		/* releasing memory */
		XMLString::release(&attr_name);
		XMLString::release(&attr_layout);
	}

	/* this function is called when finding a <layout> tag */
	void ConfigHandler::readLayoutInfo(DOMElement* elem) throw()
	{
		/* getting attributes */
		const XMLCh* xmlch_name = elem->getAttribute(ATTR_name);
		const XMLCh* xmlch_type = elem->getAttribute(ATTR_type);
		const XMLCh* xmlch_dimensions = elem->getAttribute(ATTR_dimensions);
		const XMLCh* xmlch_language = elem->getAttribute(ATTR_language);

		/* converting into char* */
		char* attr_name = XMLString::transcode(xmlch_name);
		char* attr_type = XMLString::transcode(xmlch_type);
		char* attr_dimensions = XMLString::transcode(xmlch_dimensions);
		char* attr_language = XMLString::transcode(xmlch_language);

		/* interpreting dimensions */
		std::list<int>* dims = new std::list<int>();
		std::vector<std::string> strs;
		boost::split(strs, attr_dimensions, boost::is_any_of(","));
		
		for(int i=0; i < (signed int)strs.size(); i++) {
			/* try scanning an integer */
			int d = 0;
			if(sscanf(strs[i].c_str(),"%d",&d) != 1) 
			{	
				/* it's not an integer, maybe a parameter */
				param_type_e t;
				if(config->getParameterType(strs[i].c_str(),&t) == PARAM_INT)
				{
					if(0 == config->getParameterValue(strs[i].c_str(),&d))
					{
						ERROR("Unable to read layout dimensions for layout \"" << attr_name << "\"");
						return;
					}
				}
			}
			dims->push_back(d);
		}
		
		language_e language = LG_C;

		/* interpreting language */
		if(strcmp("fortran",attr_language) == 0)
			language = LG_FORTRAN;
		else if(strcmp("C",attr_language) != 0)
		{
			WARN("Unknown language \"" << attr_language << "\", language is either \"fortran\" or \"C\". Default C language will be used.");
		}

		/* now calling the configuration object's function */
		config->setLayout(attr_name, attr_type, dims, language);
	}

	/* this function is called when finding a <event> tag */
	void ConfigHandler::readEventInfo(DOMElement* elem) throw()
	{

	}
}
