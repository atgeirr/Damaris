#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "common/Configuration.hpp"
#include "common/Util.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

using namespace boost::interprocess;
/**
 * 
 */
namespace Damaris {
	
	Client::Client(std::string* configfile, int32_t coreID)
	{
		config = new Configuration();
		try {
			msgQueue = new message_queue(open_only, config->getMsgQueueName()->c_str());
			segment = new managed_shared_memory(open_only, config->getSegmentName()->c_str());
			id = coreID;
		}
		catch(interprocess_exception &ex) {
			std::cout << ex.what() << std::endl;
		}
		LOGF("Client initialized successfully for core %d with configuration %s\n",coreID,configfile->c_str())
	}
	/**
	 *
	 */
	void* Client::alloc(std::string* varname, int32_t iteration, const Layout* datalayout)
	{
		size_t size;
		if(datalayout == (Layout*)NULL) {
			// try retrieving layout from configuration
			// TODO
			LOG("Alloc: current version of Damaris cannot make this function work without a layout\n")
			return NULL;
		} else {
			size = datalayout->getRequiredMemoryLength();
		}
		// buffer allocation
		char* buffer = static_cast<char*>(segment->allocate(size));
		
		LOG("Warning: this function is not fully implemented yet!!!\n")
		// TODO put the (varname,step,layout,ptr) in a hash table to it can be retrieved from the commit function
		return (void*)buffer;
	}
	
	int Client::commit(std::string* varname, int32_t iteration)
	{
		// TODO
		LOG("Call to commit, but this function is not implemented\n")
		return 0;
	}
	
	int Client::write(std::string* varname, int32_t iteration, const void* data, const Layout* datalayout)
	{
		const Layout* layout;
        	size_t size = 0;
        	if(datalayout == (Layout*)NULL) {
			layout = NULL;// TODO should retrieve the layout from configuration
			LOG("Write: current version of Damaris cannot make this function work without a layout\n")
			return -1;
        	} else {
        		layout = datalayout;
		}
		// allocate buffer
		size = layout->getRequiredMemoryLength();
		LOGF("Client writing %s:%ld (%d bytes)\n",varname->c_str(),(long int)iteration,(int)size);
		char* buffer = static_cast<char*>(segment->allocate(size));
		// copy data
		memcpy(buffer,data,size);
		// create message
		Message* message = new Message();
		message->sourceID = id;
		
		if(varname->length() > 63) {
			LOG("Warning, variable name length bigger than 63, will be truncated\n")
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
	
	int Client::poke(std::string* poke_name, int32_t iteration)
	{
		Message* poke = new Message();
		poke->sourceID = id;
		poke->iteration = iteration;
		poke->type = MSG_POKE;
		poke->handle = 0;
		
		if(poke_name->length() > 63) {
			LOG("Warning, poke tag length bigger than 63, will be truncated\n")
			memcpy(poke->content,poke_name->c_str(),63);
			poke->content[63] = '\0';
		} else {
			strcpy(poke->content,poke_name->c_str());
		}
		
		try {
			msgQueue->send(poke,sizeof(Message),0);
			return 0;
		} catch(interprocess_exception &e) {
			LOGF("Error while poking \"%s\", %s\n",poke_name->c_str(),e.what())
			return 1;
		}
	}
	
	Client::~Client() 
	{
		delete msgQueue;
		delete segment;
		
		LOG("Client destroyed successfuly\n")
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
	
	int DC_write(const char* varname, int32_t iteration, const void* data, const void* layout_handle)
	{
		std::string varname_str(varname);
		return client->write(&varname_str,iteration,data,(Damaris::Layout*)layout_handle);
	}
	
	void* DC_alloc(const char* varname, int32_t iteration, const void* layout_handle)
	{
		std::string varname_str(varname);
		return client->alloc(&varname_str,iteration,(Damaris::Layout*)layout_handle);
	}
	
	int DC_commit(const char* varname, int32_t iteration)
	{
		std::string varname_str(varname);
		return client->commit(&varname_str,iteration);
	}
	
	int DC_poke(const char* poke_name, int32_t iteration)
	{
		std::string poke_name_str(poke_name);
		return client->poke(&poke_name_str,iteration);
	}
	
	int DC_finalize()
	{
		delete client;
		return 0;
	}
	
	/* ======================================================================
	 Fortran Binding
	 ====================================================================== */
	
	void dc_initialize_(char* config_file_name_f, int32_t* core_id_f, int32_t* ierr_f, int config_file_name_size)
	{
		std::string config_file_name(config_file_name_f, config_file_name_size);
		client = new Damaris::Client(&config_file_name,*core_id_f);
		*ierr_f = 0;
	}
	
	void dc_write_(char* var_name_f, int32_t* iteration_f, void* data_f, int64_t* layout_handle_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name(var_name_f,var_name_size);
		*ierr_f = client->write(&var_name,*iteration_f,data_f,(Damaris::Layout*)(*layout_handle_f));
	}
	
	void dc_poke_(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size)
	{
		std::string event_name(event_name_f,event_name_size);
		*ierr_f = client->poke(&event_name,*iteration_f);
	}
	
	void dc_finalize_(int* ierr_f)
	{
		delete client;
		*ierr_f = 0;
	}
	
}
