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
 * \brief Program options parsing
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
		//("basename,B",po::value<std::string>(),"base name for output files")
		//("extension,E",po::value<std::string>(),"extension for output files")
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
		config = new Configuration(configFile,id);
	} else {
		ERROR("No configuration file provided, use --configuration=<file.xml> or -C <file.xml>");
		exit(-1);
	}
}

std::string* Options::getConfigFile()
{
	return configFile;
}

Configuration* Options::getConfiguration()
{
	return config;
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
