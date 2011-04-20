#include <iostream>
#include <list>
#include <string>
#include <signal.h>


#include "server/Options.hpp"
#include "common/Debug.hpp"
#include "server/Server.hpp"

static void sighandler(int sig);

Damaris::Server *server;

int main(int argc, char** argv)
{
	Damaris::Options* opt = new Damaris::Options(argc,argv);

	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT,  &sighandler);
	
	INFO("Starting server");
	server = new Damaris::Server(opt->getConfigFile(),opt->getID());
	server->run();
	
	INFO("Correctly terminating server\n");
	delete server;
	delete opt;
	 
	return 0;
}

static void sighandler(int sig)
{
	INFO("Kill signal caught, server will terminate");
	if(server != NULL) server->stop();
}
