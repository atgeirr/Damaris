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
#ifndef __DAMARIS_SCRIPT_ACTION_H
#define __DAMARIS_SCRIPT_ACTION_H


#include <iostream>
#include <regex>
#include <iterator>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/dict.hpp>

#include "damaris/action/ScriptAction.hpp"

namespace bp = boost::python;
namespace np = boost::python::numpy;


namespace damaris {

/**
 * ScriptAction describes an Action that wraps an interpretable script,
 * such as a Python script. This class is virtual and inherited, for
 * example, by PyAction.
 * 
 */
<xs:complexType name="Script">
		<xs:attribute name="name"      type="xs:string"  use="required"/>
		<xs:attribute name="file"      type="xs:string"  use="required"/>
		<xs:attribute name="execution" type="mdl:Exec"   use="optional" default="remote"/>
		<xs:attribute name="language"  type="mdl:Language"  use="required"/>
		<xs:attribute name="scope"     type="mdl:Scope"  use="optional" default="core"/>
		<xs:attribute name="external"  type="xs:boolean" use="optional" default="false"/>
        <xs:attribute name="frequency" type="xs:unsignedInt" use="optional" default="1" />
	</xs:complexType>
	
class PyAction : public ScriptAction , public Configurable<model::Script> {
    
    //void Output(int32_t iteration);
    
    // execution
    std::string name_ ;
    std::string language_ ;
    std::string file_ ;
    unsigned int frequency_ ;
    
	protected:
	/**
	 * Condtructor.
	 */
	PyAction(const model::Script& mdl)
	: ScriptAction(mdl.name()), Configurable<model::Script>(mdl)
	{ }

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
	static std::shared_ptr<SUPER> New(const model::Script& mdl, const std::string& name) {
        
        name_     = name ;
        language_ = mdl.language() ;
        file_     = mdl.file() ;
        frequency_= mdl.frequency() ;
        
		return std::shared_ptr<SUPER>(new PyAction(mdl), Deleter<PyAction>());
	}


};

}

#endif
