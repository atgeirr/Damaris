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
 * \file Options.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
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

/**
 * Creates a daemon process so the server can run in background 
 * (when used in standalone mode).
 */
static void daemon();

namespace Damaris {

Options::Options(int argc, char** argv)
{
	
	int id; /* id of the server */

	/* initializing the options descriptions */
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produces help message")
		("configuration,C", po::value<std::string>(), "name of the configuration file")
		("id",po::value<int>(&id),"id of the server")
		("daemon,D","starts the server as a daemon process")
		("stdout",po::value<std::string>(),"redirects stdout to a given file")
		("stderr",po::value<std::string>(),"redirects stderr to a given file")
		//("basename,B",po::value<std::string>(),"base name for output files")
		//("extension,E",po::value<std::string>(),"extension for output files")
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	/* checking the "help" command */
	if (vm.count("help")) {
		std::cout << desc << "\n";
		exit(-1);
	}

	/* checking the "daemon" command */
	if(vm.count("daemon")) {
		/* the two std streams have to be redirected in that case,
		   in order for the daemon to start properly */
		if(!(vm.count("stdout") && vm.count("stderr"))) {
			ERROR("Daemon mode used, stdout and stderr must be defined");
			exit(-1);
		}
		daemon();
	}

	/* checking if we have to redirect stdout to a file */
	if(vm.count("stdout")) {
		int fd = open((vm["stdout"].as<std::string>()).c_str(),O_RDWR|O_CREAT,0644);
		dup2(fd,1);
	}
	
	/* checking if we have to redirect stderr to a file */
	if(vm.count("stderr")) {
                int fd = open((vm["stderr"].as<std::string>()).c_str(),O_RDWR|O_CREAT,0644);
                dup2(fd,2);
        }

	/* check that we provide an id for the server */
        if (vm.count("id") == 0) {
                ERROR("Must provide a server id using --id");
                exit(-1);
        }
	
	/* now reading the configuration file and preparing the Configuration object */
	configFile = NULL;	
	if (vm.count("configuration")) {
		configFile = new std::string(vm["configuration"].as<std::string>());
		Configuration::initialize(configFile);
		config = Configuration::getInstance();
	} else {
		ERROR("No configuration file provided, use --configuration=<file.xml> or -C <file.xml>");
		exit(-1);
	}

	/* preparing the Environment object */
	env = new Environment();
	env->setID(id);
}

std::string* Options::getConfigFile()
{
	return configFile;
}

Configuration* Options::getConfiguration()
{
	return config;
}

Environment* Options::getEnvironment()
{
	return env;
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
