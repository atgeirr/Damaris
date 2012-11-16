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
 * \file BcastXML.cpp
 * \date November 2012 
 * \author Matthieu Dorier
 * \version 0.7
 */

#include "xml/BcastXML.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include "core/Debug.hpp"
#include "xml/Model.hpp"

namespace Damaris {
namespace Model {

std::auto_ptr<Damaris::Model::Simulation> BcastXML(const MPI_Comm& comm, const std::string& uri) 
{
	int rank;
	MPI_Comm_rank(comm,&rank);

	int length;
	char* buffer;
	
	if(rank == 0) {
		std::ifstream xmlfile;
		xmlfile.open(uri.c_str());
		// get length of file
		xmlfile.seekg(0, std::ios::end);
		length = xmlfile.tellg();
		xmlfile.seekg(0, std::ios::beg);
		// send length to other processes
		MPI_Bcast(&length,1,MPI_INT,0,comm);
		// allocate buffer
		buffer = new char[length];
		// read data
		xmlfile.read(buffer,length);
		xmlfile.close();
		// send data to other processes
		MPI_Bcast(buffer,length,MPI_BYTE,0,comm);
	} else {
		MPI_Bcast(&length,1,MPI_INT,0,comm);
		buffer = new char[length];
		MPI_Bcast(buffer,length,MPI_BYTE,0,comm);
	}

	// create a string
	std::string content(buffer,(size_t)length);
	delete[] buffer;
	// create an istreamstring
	std::istringstream stream(content);
	// create the XML data
	std::auto_ptr<Simulation> model;
	try {
		model = Model::simulation(stream,
				xml_schema::flags::dont_validate);
	} catch(xml_schema::exception &e) {
		ERROR(e.what());
		exit(-1);
	}
	
	return model;
}

}
}
