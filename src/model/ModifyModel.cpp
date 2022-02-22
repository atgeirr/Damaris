/*
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
#include <regex>


#include "model/ModifyModel.hpp"
#include "util/Debug.hpp"

namespace damaris {
namespace model {
    
                
void ModifyModel::SetXMLstring(std::string& input_xml) {
    config_xml_ = input_xml;
} 
   
   
   
   

ModifyModel::ModifyModel( void ) {
    converted_ = false ;
    config_xml_ =  R"V0G0N(<?xml version="1.0"?>
<simulation name="opm-flow" language="c" 
    xmlns="http://damaris.gforge.inria.fr/damaris/model">
    <architecture>
        <domains count="1"/>
        <dedicated cores="_DC_REGEX_" nodes="_DN_REGEX_"/>
        <buffer name="buffer" size="_SHMEM_BUFFER_BYTES_REGEX_" />
        <placement />
        <queue  name="queue" size="100" />
    </architecture>
    <data>
    
     <parameter name="n_elements_total"     type="int" value="1" /> 
     <parameter name="n_elements_local"     type="int" value="1" />
     <parameter name="n"     type="int" value="1" />
     
     <layout   name="zonal_layout_usmesh"             type="double" dimensions="n_elements_local"   global="n_elements_total"   comment="For the field data e.g. Pressure"  />
     <variable name="PRESSURE"    layout="zonal_layout_usmesh"     type="scalar"  visualizable="false"     unit="Pa"   centering="zonal"  store="_MYSTORE_OR_EMPTY_REGEX_" /> 
     
    </data>
    
     <storage>
      <store name="MyStore" type="HDF5">
         <option key="FileMode">Collective</option>
         <option key="XDMFMode">NoIteration</option>
         <option key="FilesPath">_PATH_REGEX_</option>
      </store>
      </storage>
      
      
    <actions>
    </actions>

      <log FileName="_PATH_REGEX_/damaris_log/exa_dbg" RotationSize="5" LogFormat="[%TimeStamp%]: %Message%"  Flush="True"  LogLevel="debug" />

</simulation>
)V0G0N";
} 


ModifyModel::ModifyModel(std::string& input_xml) {
    converted_ = false ;
    SetXMLstring(input_xml) ;     
    
}

/*  Do not include thie destructor
ModifyModel::~ModifyModel( void ) ;
{
    
}
*/

void ModifyModel::SetSimulationModel() {
    
    std::istringstream stream(config_xml_);
    
    try {
        // The creator returns a std::unique_ptr<Simulation>
        simModel_ = model::simulation(stream,
                xml_schema::flags::dont_validate);
    } catch(xml_schema::exception &e) {
        ERROR(e.what());
        ERROR("ERROR: ModifyModel::SetSimulationModel(): The XML input was :\n" << config_xml_);
        exit(-1);
    }
    this->converted_ = true ;
}
    
        

void * ModifyModel::PassModelAsVoidPtr( void ) {
    if (converted_ == true) {
      converted_ = false ;
      return (simModel_.release() );
    } else {
        
        ERROR("ERROR: ModifyModel::PassModelAsVoidPtr((): The model::simulation object has not been created- call ModifyModel::SetSimulationModel() first :\n")
        return nullptr ;
    }
}
   
   
/* Currently we need the following values to be replaced in the default constructors values for the config_xml_ string:
   _SHMEM_BUFFER_BYTES_REGEX_
   _DC_REGEX_
   _DN_REGEX_
   _PATH_REGEX_
   _MYSTORE_OR_EMPTY_REGEX_
*/
void ModifyModel::RepalceWithRegEx( std::map<std::string,std::string> find_and_replace ) {
    
    std::string config_xml_cpy(config_xml_) ;
    std::string config_xml_cpy_replaced ;
    for (auto it = find_and_replace.begin(); it != find_and_replace.end(); ++it )
    {
        std::regex e ("\\b("+it->first+")([^ ]*)") ;
        std::string replace_str = it->second + "$2" ;
        config_xml_cpy_replaced = std::regex_replace (config_xml_cpy,e,replace_str.c_str());
        config_xml_cpy = config_xml_cpy_replaced ;
    }

    config_xml_ = config_xml_cpy ;
} 

std::string & ModifyModel::GetConfigString( void )
{
    return (this->config_xml_) ;
}


std::shared_ptr<Simulation>  ModifyModel::ReturnSimulationModel( bool ignore_converted_) {
     if (converted_ == true ) {
         return std::shared_ptr<Simulation>(std::move(simModel_)) ;
         converted_ = false ;
    } else {
        return (nullptr) ;
    }
   /* if (ignore_converted_ == true){
        return (simModel_) ;
    }
    */
}


void ModifyModel::SaveXMLStringToFile(std::string filename ) {
  std::ofstream out(filename.c_str());
  out <<  this->GetConfigString() ;
  out.close();
}


Simulation * ModifyModel::GetModel( void ) {
    
    return (simModel_.get()) ;
    
}


bool ModifyModel::ReadAndBroadcastXMLFromFile(const MPI_Comm& comm, const std::string& uri) 
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
            length = 0 ;
            MPI_Bcast(&length,1,MPI_INT,0,comm);
            return false;
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
        
        if (length > 0) {
            buffer = new char[length];
            MPI_Bcast(buffer,length,MPI_BYTE,0,comm);
        } else {
            return false;
        }
    }

     config_xml_.clear() ;
    // create a string
     config_xml_.assign( buffer ) ;
     delete[] buffer;
     return true ;
} 


//std::shared_ptr<Simulation> ModifyModel::ReturnSharedModelPtr( void ) {
//    
//}

}
}
