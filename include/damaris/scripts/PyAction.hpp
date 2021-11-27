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
#ifndef __DAMARIS_PYACTION_H
#define __DAMARIS_PYACTION_H


#include "damaris/action/ScriptAction.hpp"

#include <iostream>
#include <regex>
#include <iterator>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/dict.hpp>

namespace bp = boost::python;
namespace np = boost::python::numpy;

/**
 * ScriptAction describes an Action that wraps an interpretable script,
 * such as a Python script. This class is virtual and inherited, for
 * example, by PyAction.
 * 
    <xs:complexType name="Script">
		<xs:attribute name="name"      type="xs:string"  use="required"/>
		<xs:attribute name="file"      type="xs:string"  use="required"/>
		<xs:attribute name="execution" type="mdl:Exec"   use="optional" default="remote"/>
		<xs:attribute name="language"  type="mdl:Language"  use="required"/>
		<xs:attribute name="scope"     type="mdl:Scope"  use="optional" default="core"/>
		<xs:attribute name="external"  type="xs:boolean" use="optional" default="false"/>
        <xs:attribute name="frequency" type="xs:unsignedInt" use="optional" default="1" />
	</xs:complexType>
*/

namespace damaris {



class PyAction : public ScriptAction {
    
    //void Output(int32_t iteration);
    friend class Deleter<PyAction>;
     
    // Data obtained from the XML model file
    std::string name_ ;
    std::string language_ ;
    std::string file_ ;
    unsigned int frequency_ ;
    
    /**
    * The dictionary that holds the Python environment
    */
    bp::dict locals;
    /**
    * The dictionary that wil lstroe the created numpy arrays that 
    * wrap the Damaris data. The dictionary will be added to the Python 
    * local dictionary
    */
    bp::dict damarisData;
    
    /**
    * used to hold the __main__ module and obtain the globals dict
    */
    bp::object main;
    
    /**
    * used to hold the globals dict which is obtained from the __main__ module
    */
    bp::object globals;
    
    /**
    * String of Python code used to remove datasets from Pyhton environment when the 
    * Damris data they use  is invalidated/deleted
    */
    std::string regex_string_with_python_code; 
    
	protected:
	/**
	 * Constructor. Initailizes Pyhton and boost::numpy
	 */
	PyAction(const model::Script& mdl);

    
    /**
    * Uses boost::Numpy data types to convert a Damaris variable 
    * (as given in the XML model) to a numpy data type.
    */
     bool GetNumPyType(model::Type mdlType, np::dtype &dt);


    /**
    * Creates a name to be used as key in a Python dictionary to reference the data 
    *  within a Python script.
    *  The string is a concatenation of the variable name (defined in XML file) and adds:
    *  - A block string "_Px", where x is the Damaris client global rank of the 
    *    source of the data
    *  - And, if the data is multi-domain (as set in XML file) then adds:
    *    A domain string "_By" where y is the domain ID
    * 
    */
    std::string GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b);


    /**
    * Uses boost::Numpy data types to convert a Damaris variable 
    * (as given in the XML model) to a numpy data type.
    */
    bool PassDataToPython(int iteration );


	/**
	 * Destructor.
	 */
	virtual ~PyAction() {}

	public:	
    
	/**
	 * \see damaris::Action::operator()
	 */
	virtual void Call(int32_t sourceID, int32_t iteration,
				const char* args = NULL) ;

	/**
	 * Tells if the action can be called from outside the simulation.
	 */
	virtual bool IsExternallyVisible() const { 
		return GetModel().external(); 
	}

	/**
	 * \see Action::GetExecLocation
	 */
	virtual model::Exec GetExecLocation() const {
		return GetModel().execution();
	}

	/**
	 * \see Action::GetScope
	 */
	virtual model::Scope GetScope() const {
		return GetModel().scope();
	}

	/**
	 * Creates a new instance of an inherited class of ScriptAction 
	 * according to the "language" field in the description.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Script& mdl) {
        
		return std::shared_ptr<SUPER>(new PyAction(mdl), Deleter<PyAction>());
	}


};

}

#endif
