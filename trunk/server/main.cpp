#include <iostream>
#include <list>
#include <string>
#include <signal.h>

#include <xercesc/util/PlatformUtils.hpp>

#include "server/Options.hpp"
#include "common/Debug.hpp"
#include "server/Server.hpp"

using namespace xercesc;

static void sighandler(int sig);

Damaris::Server *server;

int main(int argc, char** argv)
{
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException& e) {
		ERROR("XML initialization exception: " << e.getMessage());
		return 1;
	}

	INFO("Parsing program options");
        Damaris::Options* opt = new Damaris::Options(argc,argv);

	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT,  &sighandler);
	
	INFO("Initializing server");
	server = new Damaris::Server(opt->getConfiguration());
	delete opt;
	INFO("Starting server");
	server->run();
	
	INFO("Correctly terminating server\n");
	delete server;

	XMLPlatformUtils::Terminate();
 
	return 0;
}

static void sighandler(int sig)
{
		INFO("Kill signal caught, server will terminate");
	if(server != NULL) server->stop();
}
