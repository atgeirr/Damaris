#include <iostream>
#include <list>
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/program_options.hpp>

#include "server/Options.hpp"
#include "common/Debug.hpp"

namespace po = boost::program_options;

static void daemon();

namespace Damaris {

Options::Options(int argc, char** argv)
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
		("basename,B",po::value<std::string>(),"base name for output files")
		("extension,E",po::value<std::string>(),"extension for output files")
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		exit(-1);
	}

	if(vm.count("daemon")) {
		if(!(vm.count("stdout") && vm.count("stderr"))) {
			ERROR("Daemon mode used, stdout and stderr must be defined");
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
	
	configFile = NULL;
	if (vm.count("configuration")) {
		configFile = new std::string(vm["configuration"].as<std::string>());
	}
}

std::string* Options::getConfigFile()
{
	return configFile;
}

int Options::getID()
{
	return id;
}
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
