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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <iostream>
#include <list>
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mpi.h>
#include <boost/program_options.hpp>

#include "xml/Model.hpp"
#include "server/Options.hpp"
#include "common/Debug.hpp"

namespace po = boost::program_options;

namespace Damaris {

Options::Options(int argc, char** argv)
{
	
	/* initializing the options descriptions */
	po::options_description desc("Allowed options");
	desc.add_options()
		("configuration,C", po::value<std::string>(&configFile), 
				"name of the configuration file")
		("stdout",po::value<std::string>(),"redirects stdout to a given file")
		("stderr",po::value<std::string>(),"redirects stderr to a given file")
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// checking if we have to redirect stdout to a file 
	if(vm.count("stdout")) {
		if(not freopen((vm["stdout"].as<std::string>()).c_str(),"w",stdout))
		{
			ERROR("Unable to redirect stdout");
		}
	}
	
	// checking if we have to redirect stderr to a file 
	if(vm.count("stderr")) {
		if(not freopen((vm["stderr"].as<std::string>()).c_str(),"w",stderr))
		{
			ERROR("Unable to redirect stderr");
		}
        }

	// now reading the configuration file and preparing the Configuration object 
	if (vm.count("configuration") == 0) {
		ERROR("No configuration file provided," 
			<< " use --configuration=<file.xml> or -C <file.xml>");
		exit(-1);
	}
}

const std::string& Options::getConfigFile()
{
	return configFile;
}

}

