#include <iostream>
#include <string>
#include "client/Client.hpp"      

int main(int argc, char** argv) 
{
	int id = 0;
	float mydata[64][16][2];

	if(argc != 2) {
		std::cout << "Usage: ./test_c <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);

	Damaris::Client* client = new Damaris::Client(config,id);

	int i,j,k;
	for(i = 0; i < 64; i++) {
	for(j = 0; j < 16; j++) {
	for(k = 0; k <  2; k++) {
		mydata[i][j][k] = i*j*k;
	}
	}
	}

	std::string varname("my variable");
	std::string event("my event");

	client->write(varname,0,mydata);
	client->signal(event,0);

	delete client;
	return 0;
}
