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
 * \file Client.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * \see Client.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "common/Debug.hpp"
#include "common/Configuration.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	Client::Client(std::string* configfile, int32_t coreID)
	{
		/* creates the configuration object from the configuration file */
		config = new Configuration(configfile);
		id = coreID;
		/* initializes the shared structures */
		try {
			msgQueue = new message_queue(open_only, config->getMsgQueueName()->c_str());
			segment = new managed_shared_memory(open_only, config->getSegmentName()->c_str());
			INFO("Client initialized successfully for core " << id << " with configuration " << *configfile);
		}
		catch(interprocess_exception &ex) {
			std::cout << ex.what() << std::endl;
		}
	}
	
	void* Client::alloc(std::string* varname, int32_t iteration)
	{
		// TODO : this function is not fully implemented
		return NULL;

		size_t size;
		Layout* layout = config->getVariableLayout(varname->c_str());
		if(layout == (Layout*)NULL) {
			// try retrieving layout from configuration
			// TODO
			ERROR("The current version of Damaris cannot make this function work without a layout");
			return NULL;
		} else {
			size = layout->getRequiredMemoryLength();
		}
		// buffer allocation
		char* buffer = static_cast<char*>(segment->allocate(size));
		
		INFO("Warning: this function is not fully implemented yet!!!");
		// TODO put the (varname,step,layout,ptr) in a hash table so 
		// it can be retrieved from the commit function
		return (void*)buffer;
	}
	
	int Client::commit(std::string* varname, int32_t iteration)
	{
		// TODO retrieve the variable name from a hash table somewhere
		// then send a write-notification
		ERROR("This function is not implemented");
		return -1;
	}
	
	int Client::write(std::string* varname, int32_t iteration, const void* data)
	{
		/* check that the variable is know in the configuration */
		Layout* layout = config->getVariableLayout(varname->c_str());
		size_t size = 0;
        	if(layout == (Layout*)NULL) {
			ERROR("No layout found in configuration for variable \""<< varname->c_str() << "\" or variable not defined");
			return -1;
        	}

		// get required size
		size = layout->getRequiredMemoryLength();
		if(size == 0) {
			ERROR("Layout has size 0");
			return -2;
		}

		// allocate buffer
		char* buffer = NULL;
		try {		
			buffer = static_cast<char*>(segment->allocate(size));
		} catch (std::bad_alloc &e) {
			buffer = NULL;
		}
		if(buffer == NULL) {
			ERROR("While writing \"" << varname->c_str() << "\", allocation failed");
			return -3;
		}
		
		// copy data
		memcpy(buffer,data,size);
		// create message
		Message* message = new Message();
		message->sourceID = id;
		
		if(varname->length() > 63) {
			WARN("Variable name length bigger than 63, will be truncated");
			memcpy(message->content,varname->c_str(),63);
			message->content[63] = '\0';
		} else {
			strcpy(message->content,varname->c_str());
		}
		
		LayoutFactory::serialize(layout, message->layoutInfo);
		
		message->iteration = iteration;
		message->type = MSG_VAR;
		message->handle = segment->get_handle_from_address(buffer);
		// send message
		msgQueue->send(message,sizeof(Message),0);
		// free message
		INFO("Variable \"" << varname->c_str() "\" has been written");
		delete message;
		
		return size;
	}
	
	int Client::signal(std::string* signal_name, int32_t iteration)
	{
		Message* sig = new Message();
		sig->sourceID = id;
		sig->iteration = iteration;
		sig->type = MSG_SIG;
		sig->handle = 0;
		
		if(signal_name->length() > 63) {
			WARN("Signal's name length bigger than 63, will be truncated");
			memcpy(sig->content,signal_name->c_str(),63);
			sig->content[63] = '\0';
		} else {
			strcpy(sig->content,signal_name->c_str());
		}
		
		try {
			msgQueue->send(sig,sizeof(Message),0);
			return 0;
		} catch(interprocess_exception &e) {
			ERROR("Error while sending event \"" << *signal_name << "\", " << e.what());
			return -1;
		}
	}

	int Client::getParameter(std::string* paramName, void* buffer)
	{
		Types::basic_type_e t;
		if(config->getParameterType(paramName->c_str(),&t)) 
		{
			config->getParameterValue(paramName->c_str(),buffer);
			return 0;
		} else {
			ERROR("Parameter \""<< paramName->c_str() <<"\"not found in the configuration");
			return -1;
		}
	}
	
	Client::~Client() 
	{
		delete msgQueue;
		delete segment;
		delete config;
		
		INFO("Client destroyed successfuly");
	}
	
}

