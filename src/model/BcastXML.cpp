/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <mpi.h>

#include "util/Debug.hpp"
#include "model/BcastXML.hpp"

namespace damaris {
namespace model {

std::shared_ptr<Simulation> BcastXML(const MPI_Comm& comm, const std::string& uri) 
{
    int rank;
    MPI_Comm_rank(comm,&rank);

    int length;
    char* buffer;
    
    if(rank == 0) {
        std::ifstream xmlfile;
        xmlfile.open(uri.c_str());
        
        if(xmlfile.fail()) {
            ERROR("Fail to open configuration file " << uri);
            return (nullptr) ;
            // MPI_Abort(comm,-1);
            // exit(-1);
        }

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
    std::shared_ptr<Simulation> model;
    try {
        model = model::simulation(stream,
                xml_schema::flags::dont_validate);
    } catch(xml_schema::exception &e) {
        ERROR(e.what());
        ERROR("The XML input was :\n" << content << "\n");
        exit(-1);
    }
    
    return model;
}

std::shared_ptr<Simulation> LoadXML(const std::string& uri)
{
    std::unique_ptr<Simulation> model;

    try {
        std::ifstream xmlfile;
        xmlfile.open(uri.c_str());
        model = model::simulation(xmlfile,
                xml_schema::flags::dont_validate);
        xmlfile.close();
    } catch(xml_schema::exception &e) {
        ERROR(e.what());
        ERROR("The XML file was : " << uri << "\n");
        exit(-1);
    }
    return std::shared_ptr<Simulation>(std::move(model));
}

}
}
