#include <iostream>
#include <string>
#include "client/Client.hpp"      

int main(int argc, char** argv) 
{
	int id = 42;
	char* mydata = NULL;

	if(argc != 2) {
		std::cout << "Usage: ./test_alloc_c <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);

	Damaris::Client* client = new Damaris::Client(config,id);
	
	std::string varname("my string");
	std::string eventname("my event");

	mydata = (char*)client->alloc(varname,1);
	if(mydata == NULL) {
		std::cout << "Error when allocating buffer" << std::endl;
		exit(0);
	}

	strcpy(mydata,"this is written in shared memory");
		
	client->commit(varname,1);
	client->signal(eventname,1);
	delete client;
	return 0;
}
