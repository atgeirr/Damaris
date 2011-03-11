#include <iostream>
#include <list>
#include <string>
#include <signal.h>

#include <boost/program_options.hpp>
//#include <mpi.h>

#include "common/Debug.hpp"
#include "common/Util.hpp"
#include "server/Server.hpp"

namespace po = boost::program_options;

using namespace boost::interprocess;

static void sighandler(int sig);
static void daemon();

Damaris::Server *server;

int main(int argc, char** argv)
{
	int id;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("configuration,C", po::value<std::string>(), "name of the configuration file")
		("id",po::value<int>(&id)->default_value(0),"id of the node")
		("daemon,D","start the server as daemon process")
		("stdout",po::value<std::string>(),"redirect stdout to a given file")
		("stderr",po::value<std::string>(),"redirect stderr to a given file")
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	if(vm.count("daemon")) {
		if(!(vm.count("stdout"))) {
			ERROR("Daemon mode used, stdout must be defined");
			exit(-1);
		}
		if(!(vm.count("stderr"))) {
			ERROR("Daemon mode used, stderr must be defined");
			exit(-1);
		}
		daemon();
	}

	if(vm.count("stdout")) {
		int fd = open((vm["stdout"].as<std::string>()).c_str(),O_RDWR|O_CREAT,0644);
		dup2(fd,1);
	}
	
	if(vm.count("stderr")) {
                int fd = open((vm["stderr"].as<std::string>()).c_str(),O_RDWR|O_CREAT,0644);
                dup2(fd,2);
        }
	
	std::string* config = NULL;
	if (vm.count("configuration")) {
		config = new std::string(vm["configuration"].as<std::string>());
	} else {
		//std::cout << "No configuration file specified.\n";
		//return 1;
		// TODO
		config = new std::string("");
	}
	
//	MPI_Init(&argc,&argv);

//	int rank, size;
	
//	MPI_Comm_size(MPI_COMM_WORLD,&size); 
//	MPI_Comm_rank(MPI_COMM_WORLD,&rank); 

	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT,  &sighandler);
	
	INFO("Starting server");
	server = new Damaris::Server(config,id);
	server->run();
	
	INFO("Correctly terminating server\n");
	delete server;
//	MPI_Finalize();
	 
	return 0;
}

static void sighandler(int sig)
{
	INFO("Kill signal caught, server will terminate");
	if(server != NULL) server->stop();
}

static void daemon()
{
	int i;
	if(getppid()==1)
		return;

	i=fork();
	if (i<0) exit(1);
	if (i>0) exit(0);

	setsid();
}
