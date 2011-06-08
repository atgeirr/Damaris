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

#ifdef __ENABLE_FORTRAN
	#include "common/FCMangle.h"
#endif

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
		}
		catch(interprocess_exception &ex) {
			std::cout << ex.what() << std::endl;
		}
		INFO("Client initialized successfully for core " << id << " with configuration " << *configfile);
	}
	
	void* Client::alloc(std::string* varname, int32_t iteration)//, const Layout* datalayout)
	{
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
		return 0;
	}
	
	int Client::write(std::string* varname, int32_t iteration, const void* data)
	{
		/* check that the variable is know in the configuration */
		Layout* layout = config->getVariableLayout(varname->c_str());
		size_t size = 0;
        	if(layout == (Layout*)NULL) {
			ERROR("No layout found in configuration for variable "<< varname->c_str());
			return -1;
        	}
		// allocate buffer
		size = layout->getRequiredMemoryLength();
		if(size == 0) {
			ERROR("Layout has size 0");
			return -1;
		}
		
		char* buffer = static_cast<char*>(segment->allocate(size));
		// copy data
		memcpy(buffer,data,size);
		// create message
		Message* message = new Message();
		message->sourceID = id;
		
		if(varname->length() > 63) {
			ERROR("Warning: variable name length bigger than 63, will be truncated");
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
			ERROR("Warning: signal tag length bigger than 63, will be truncated");
			memcpy(sig->content,signal_name->c_str(),63);
			sig->content[63] = '\0';
		} else {
			strcpy(sig->content,signal_name->c_str());
		}
		
		try {
			msgQueue->send(sig,sizeof(Message),0);
			return 0;
		} catch(interprocess_exception &e) {
			ERROR("Error while poking " << *signal_name << ", " << e.what());
			return 1;
		}
	}

	int Client::getParameter(std::string* paramName, void* buffer)
	{
		basic_type_e t;
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


/* ====================================================================== 
 C Binding
 ====================================================================== */

extern "C" {
	
	Damaris::Client *client;

	int DC_initialize(const char* configfile, int32_t core_id)
	{
		std::string config_str(configfile);
		client = new Damaris::Client(&config_str,core_id);
		return 0;
	}
	
	int DC_write(const char* varname, int32_t iteration, const void* data)
	{
		std::string varname_str(varname);
		return client->write(&varname_str,iteration,data);
	}
	
	void* DC_alloc(const char* varname, int32_t iteration)
	{
		std::string varname_str(varname);
		return client->alloc(&varname_str,iteration);
	}
	
	int DC_commit(const char* varname, int32_t iteration)
	{
		std::string varname_str(varname);
		return client->commit(&varname_str,iteration);
	}
	
	int DC_signal(const char* signal_name, int32_t iteration)
	{
		std::string signal_name_str(signal_name);
		return client->signal(&signal_name_str,iteration);
	}

	int DC_get_parameter(const char* param_name, void* buffer)
	{
		std::string paramName(param_name);
		return client->getParameter(&paramName,buffer);
	}

	int DC_finalize()
	{
		delete client;
		return 0;
	}
#ifdef __ENABLE_FORTRAN	
/* ======================================================================
 Fortran Binding
 ====================================================================== */
	
	void FC_FUNC_GLOBAL(df_initialize,DF_INITIALIZE)
		(char* config_file_name_f, int32_t* core_id_f, int32_t* ierr_f, int config_file_name_size)
	{
		std::string config_file_name(config_file_name_f, config_file_name_size);
		client = new Damaris::Client(&config_file_name,*core_id_f);
		*ierr_f = 0;
	}
	
	void FC_FUNC_GLOBAL(df_write,DF_WRITE)
		(char* var_name_f, int32_t* iteration_f, void* data_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name(var_name_f,var_name_size);
		*ierr_f = client->write(&var_name,*iteration_f,data_f);
	}
	
	void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
		(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size)
	{
		std::string event_name(event_name_f,event_name_size);
		*ierr_f = client->signal(&event_name,*iteration_f);
	}
	
	void FC_FUNC_GLOBAL(df_get_parameter,DF_GET_PARAMETER)
		(char* param_name_f, void* buffer_f, int* ierr_f, int param_name_size)
	{
		std::string paramName(param_name_f,param_name_size);
		*ierr_f = client->getParameter(&paramName,buffer_f);
	}
	void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
		(int* ierr_f)
	{
		delete client;
		*ierr_f = 0;
	}
#endif
}
