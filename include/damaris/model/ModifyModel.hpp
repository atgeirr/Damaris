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
#include <fstream>
#include <map>

#include <mpi.h>

#include "damaris/model/Model.hpp"

namespace damaris {
namespace model {
    
/** 
 * The class ModifyModel alows a user to provide an string version of an XML 
 * configuration file and then modify then XML model that is created so that 
 * the Damaris configuration can spcified prorammatically.
 */
class ModifyModel {
          
    private:
        std::string config_xml_ ; /*!< A string of XML that (will) conform to the Damaris XSD model. */
        std::unique_ptr<Simulation> simModel_ ; /*!< The Damaris XML model that will be created rom the config_xml_ string */
        bool converted_ ;          /*!< is true if the string XML value has been converted to the std::unique_ptr<Simulation> value */
                     
    public:

        /**
        * Constructor (Default). Initializes config_xml_ string to a basic XML string based that contains
        * REGEX substrings to replace (good for testing)
        */
        ModifyModel( void ) ;
        
        /**
        * Constructor. Accepts a string of XML that is stored within the config_xml_ class member. This string
        * will initialize the the Damaris XML *Simulation* model.
        *
        * \param[in] input_xml : A string of XML that (will) conform to the Damaris XSD model.
        */
        ModifyModel(std::string& input_xml);
        
         /**
        * Copies the input string to the config_xml_ class member.
        *
        * \param[in] input_xml : A string of XML that needs to (after any preprocessing) conform to the Damaris XSD model.
        */
        void SetXMLstring(std::string& input_xml) ;       
        /**
        * Repalces keys in the XML string with  values found in the std::map
        * 
        * \param[in] find_and_replace : std::map of string,string pairs where the key (->first) is the string to
        *                                find and the value (->second) is the string with which to replace) 
        */
        void RepalceWithRegEx( std::map<std::string,std::string> find_and_replace ) ;
        
        /**
         * Returns the currrent XML based Damaris configuration string
         */
        std::string & GetConfigString( void ) ;
        
        /**
         * Save the current XML string to a file
         * 
         *  * \param[in] filename :  The filename to save the config_xml_XML string to 
         */
        void SaveXMLStringToFile(std::string filename ) ;
        
        
        /**
         * Loads an XML file in the rank 0 of the given MPI communicator, broadcast
         * its content to other processes that will copy it to the config_xml_ string.
         *
         * \param[in] comm : MPI communicator gather all processess that need to 
         *                   load the XML file.
         * \param[in] uri  : Path and filename of the XML file.
        */
        bool ReadAndBroadcastXMLFromFile(const MPI_Comm& comm, const std::string& uri) ;
    
        
    protected:
        
        /**
        *  Uses ModifyModel::config_xml_ string to initialize the the Damaris XML *Simulation* model. 
        *  Only call this method after any required preprocessing has been done to the XML string.
        *
        */
        void SetSimulationModel( void ) ;
        
        
        /**
        *  Returns the *Simulation* model as a shared_ptr<>. This class will then not contain a Simulation object, 
        *  so SetSimulationModel() will nedd to be called again to generate one.
        *  Only call this method after any required preprocessing has been done.
        * 
        * \param[in] ignore_converted_ : if true then returns the  simModel_ object before it has been initialized by the XML text. This is experimental.
        */
        std::shared_ptr<Simulation> ReturnSimulationModel( bool ignore_converted_=false  ) ;
        
        /**
        * Converts the Damaris XML *Simulation* model to a void * pointer that can be used in 
        * Damaris C API call damaris_initialize( void *, MPI_Comm ). Once this call is made, the 
        * Damaris library is initialized and cannot be subsequently modified and the current 
        * std::unique_ptr<Simulation> object has been std::move()'ed to the Damaris library'
        */
        void * PassModelAsVoidPtr( void ) ;
        
        
        /**
        * Allows access to the  the Damaris XML *Simulation* model so it can be modified using the XSD geenrated API
        * N.B. Used for testing, not used in Damaris public API
        */
        Simulation * GetModel( void ) ;
        
};  // end of  class ModifyModel
    
}
}
