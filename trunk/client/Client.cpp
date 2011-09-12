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
#include "client/ClientConfiguration.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	Client::Client(std::string* configfile, int32_t coreID)
	{
		/* creates the configuration object from the configuration file */
		ClientConfiguration::initialize(configfile);
		config = ClientConfiguration::getInstance();
		id = coreID;
		/* initializes the shared structures */
		try {
			msgQueue = new message_queue(open_only, config->getMsgQueueName()->c_str());
			segment = SharedMemorySegment::open(posix_shmem,config->getSegmentName()->c_str());
					//new managed_shared_memory(open_only, config->getSegmentName()->c_str());
			INFO("Client initialized successfully for core " << id << " with configuration " << *configfile);
		}
		catch(interprocess_exception &ex) {
			std::cout << ex.what() << std::endl;
		}
		variables = new MetadataManager(segment);
	}
	
	void* Client::alloc(std::string* varname, int32_t iteration)
	{
		size_t size = 0;
		char* buffer = NULL;

		Layout* layout = config->getVariableLayout(varname->c_str());
		if(layout == (Layout*)NULL) {
			ERROR("Unknown variable \"" << varname->c_str() << "\" or undefined layout for this variable");	
			return NULL;
		} else {
			Variable* allocated = variables->get(varname,iteration,id);
			if(allocated != NULL) return (void*)(allocated->data);
			size = layout->getRequiredMemoryLength();
		}
		// buffer allocation
		buffer = static_cast<char*>(segment->allocate(size));
		Variable allocated(*varname,iteration,id,layout,buffer);
		variables->put(allocated);
		
		return (void*)buffer;
	}
	
	int Client::commit(std::string* varname, int32_t iteration)
	{
		Variable* allocated = variables->get(varname,iteration,id);
		if(allocated == NULL)
			return -1;
		
		// create notification message
		Message* message = new Message();
		message->sourceID = id;

		if(varname->length() > 63) {
			WARN("Variable name length bigger than 63, will be truncated");
			memcpy(message->content,varname->c_str(),63);
			message->content[63] = '\0';
		} else {
			strcpy(message->content,varname->c_str());
		}
		
		Layout* layout = allocated->layout;
		LayoutFactory::serialize(layout, message->layoutInfo);

		char* buffer = (char*)(allocated->data);
		message->iteration = iteration;
		message->type = MSG_VAR;
		message->handle = segment->getHandleFromAddress((void*)buffer);
                // send message
		msgQueue->send(message,sizeof(Message),0);
                // free message
		INFO("Variable \"" << varname->c_str() << "\" has been commited");
		delete message;

		// remove variable from metadata
		allocated->data = NULL; // prevent metadata manager from deleting content
		variables->remove(*allocated);

                return 0;
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
		message->handle = segment->getHandleFromAddress(buffer);
		// send message
		msgQueue->send(message,sizeof(Message),0);
		// free message
		INFO("Variable \"" << varname->c_str() << "\" has been written");
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
		
	int Client::killServer()
	{
		static int killed;
		if(!killed) {
			std::string sig("#kill");
			int res = signal(&sig,0);
			if(res == 0) killed = 1;
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server.");
			return -1;
		}
	}
	
	Client::~Client() 
	{
		delete msgQueue;
		delete segment;
		config->finalize();
		
		INFO("Client destroyed successfuly");
	}
	
}

