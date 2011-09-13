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

/**
 * \file ConfigHandler.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#ifndef __DAMARIS_CFGHANDLER_H
#define __DAMARIS_CFGHANDLER_H

#include <string>
#include <stdexcept>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

#include "common/Debug.hpp"
#include "common/Group.hpp"
#include "common/Configuration.hpp"

using namespace xercesc;

namespace Damaris {

/**
 * This class serves as factory to build a Configuration object
 * from an XML file using Xerces-C XML library.
 */
class ConfigHandler {
		
	private:
		XercesDOMParser *configFileParser; 
		Configuration* config;
		Group* currentGroup;

		bool nodeParsed;

		/* all these variables are used for identifying tag names */
		XMLCh* TAG_simulation;
		XMLCh* TAG_nodes;
		XMLCh* TAG_nodes_cores;
		XMLCh* TAG_nodes_clients;
		XMLCh* TAG_nodes_buffer;
		XMLCh* TAG_nodes_queue;
		XMLCh* TAG_data;
		XMLCh* TAG_data_parameter;
		XMLCh* TAG_data_variable;
		XMLCh* TAG_data_group;
		XMLCh* TAG_data_layout;
		XMLCh* TAG_actions;
		XMLCh* TAG_actions_event;
		
		/* all these variables are used for identifying attributes' names */
		XMLCh* ATTR_name;
		XMLCh* ATTR_size;
		XMLCh* ATTR_count;
		XMLCh* ATTR_type;
		XMLCh* ATTR_layout;
		XMLCh* ATTR_dimensions;
		XMLCh* ATTR_language;
		XMLCh* ATTR_value;
		XMLCh* ATTR_action;
		XMLCh* ATTR_using;
		XMLCh* ATTR_enabled;
		XMLCh* ATTR_shmem;

		void readNodesConfig(DOMElement* elem) 		throw();
		void readDataConfig(DOMElement* elem) 		throw();
		void readDataGroups(DOMElement* elem)		throw();
		void readActionsConfig(DOMElement* elem)	throw();
		void readParameterInfo(DOMElement* elem) 	throw();
		void readVariableInfo(DOMElement* elem) 	throw();
		void readGroupInfo(DOMElement*)			throw();
		void readLayoutInfo(DOMElement* elem)		throw();
		void readEventInfo(DOMElement* elem)		throw();
	public:
		/** 
		 * The constructor takes a Configuration object as argument,
		 * this object will be filled when calling readConfigFiles. 
		 * \param[in,out] c : pointer to the Configuration object to be filled.
		 */
		ConfigHandler(Configuration* c);

		/**
		 * Destructor.
		 */
		~ConfigHandler();

		/**
		 * Reads a configuration file and fill the Configuration object that
		 * has been given to the constructor.
		 * \param[in] cfgFile : name of the configuration file.
		 */
		void readConfigFile(std::string *cfgFile);
	};

}

#endif
