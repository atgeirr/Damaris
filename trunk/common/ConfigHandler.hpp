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
#include "common/Configuration.hpp"

using namespace xercesc;

namespace Damaris {

	class ConfigHandler {
		
	private:
		XercesDOMParser *configFileParser;
		Configuration* config;

		XMLCh* TAG_simulation;
		XMLCh* TAG_nodes;
		XMLCh* TAG_nodes_cores;
		XMLCh* TAG_nodes_buffer;
		XMLCh* TAG_nodes_queue;
		
		XMLCh* ATTR_name;
		XMLCh* ATTR_size;
		XMLCh* ATTR_count;

		void readNodesConfig(DOMElement* elem) throw();
	public:
		ConfigHandler(Configuration* c);
		~ConfigHandler();
		void readConfigFile(std::string *cfgFile);
	};

}

#endif
