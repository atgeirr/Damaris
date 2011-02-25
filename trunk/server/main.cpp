#include <iostream>
#include <list>
#include <signal.h>

#include "common/Util.hpp"
#include "server/Server.hpp"

using namespace boost::interprocess;

static void usage(char**);
static void sighandler(int sig);

Damaris::Server *server;

int main(int argc, char** argv)
{
	if(argc != 2) {
		usage(argv);
		exit(-1);
	}
	
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT,  &sighandler);
	
	LOG("Starting server\n")
	std::string config(argv[1]);
	server = new Damaris::Server(&config);
	server->run();
	
	LOG("Correctly terminating server\n")
	delete server;
	return 0;
}

static void usage(char** argv)
{
	std::cout << "Usage: " << argv[0] << " config.xml" << std::endl;
}

static void sighandler(int sig)
{
	std::cerr << std::endl;
	LOGF("Signal %d caught, server will terminate...\n",sig);
	server->stop();
}