#include <iostream>
#include <set>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "damaris/model/ModifyModel.hpp"
#include "damaris/model/BcastXML.hpp"
#include "damaris/action/ActionManager.hpp"
#include "damaris/env/Environment.hpp"


namespace damaris {

/* I am adding the derived class so I can get access to the protected methods that have been hidden
 * so that they are not used in any final user code (for now)
 */
namespace model {
class ModifyModelDerived : public ModifyModel {
    public:  
        using ModifyModel::ModifyModel;
        using ModifyModel::SetSimulationModel;
        using ModifyModel::ReturnSimulationModel;
        using ModifyModel::PassModelAsVoidPtr;
        using ModifyModel::GetModel;
         
} ;
}

class TestModifyModel : public CppUnit::TestFixture {
	
private:
	static bool initialized;
public:
	TestModifyModel() {

	}

	virtual ~TestModifyModel() {
		//Environment::Finalize();
		//initialized = false;
	}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("ModifyModel");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModel>(
				"Creates a ModifyModel class and preprocess using REGEX simplified xml ",
				&TestModifyModel::CallCreateModifyModelandRegex));
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModel>(
				"Creates a ModifyModel class and preprocess using REGEX, then returns the model::simulation and tests values of model (mdl->architecture().buffer().size()) ",
				&TestModifyModel::CallCreateModifyModelandSimulation));
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModel>(
				"Creates a ModifyModel class and preprocess using REGEX, then returns the model::simulation via the void * release from the unique_ptr<Simulation> ",
				&TestModifyModel::CallCreateModifyModelandSimulationViaVoid));
		
		return suiteOfTests;
	}

protected:
	
	void CallCreateModifyModelandRegex() {
        std::string input_xml = R"V0G0N( 
_SHMEM_BUFFER_BYTES_REGEX_ _SHMEM_BUFFER_BYTES_REGEX_
_DC_REGEX_ _SHMEM_BUFFER_BYTES_REGEX_
_PATH_REGEX_
_MYSTORE_OR_EMPTY_REGEX_
)V0G0N";
        std::string expected = R"V0G0N( 
67108864 67108864
2 67108864
outputdir
MyStore
)V0G0N";
		damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived(input_xml);
        // std::cout << "Input string: " << std::endl  << myMod.GetConfigString()  << std::endl ;
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","2"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_","MyStore"},
        };
        myMod.RepalceWithRegEx(find_replace_map);
        
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing REGEXed string with expected",
              expected,
 myMod.GetConfigString() ) ;
	}
	
	void CallCreateModifyModelandSimulation() {
		damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived(); // this creator is pre-defined with a Simulation XML string
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","1"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_","MyStore"},
        };
        myMod.RepalceWithRegEx(find_replace_map);
        myMod.SetSimulationModel() ;   
        
        std::shared_ptr<model::Simulation> mdl = myMod.ReturnSimulationModel() ;
        //std::shared_ptr<model::Simulation>  mdl = model::LoadXML("simulation.xml");
        unsigned long int buffer_size = static_cast<unsigned long>(mdl->architecture().buffer().size()) ;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing REGEXed mdl->architecture().buffer().size()", 67108864ul, buffer_size
 ) ;
       
		initialized = false; 
	}
	
	void CallCreateModifyModelandSimulationViaVoid() {
		damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived();  // this creator is pre-defined with a Simulation XML string
        std::cout << "Input string: " << std::endl  << myMod.GetConfigString()  << std::endl ;
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","1"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_","MyStore"},
        };
        myMod.RepalceWithRegEx(find_replace_map);
        myMod.SetSimulationModel() ;   
        
        // casting the returned void * back to a shared_ptr<Simulation>
        std::shared_ptr<model::Simulation> mdl  = std::shared_ptr<model::Simulation>( static_cast<model::Simulation *>( myMod.PassModelAsVoidPtr() )) ; 

        unsigned long int buffer_size = static_cast<unsigned long>(mdl->architecture().buffer().size()) ;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing REGEXed mdl->architecture().buffer().size()", 67108864ul, buffer_size
 ) ;
       
		initialized = false;
	}
	
};

bool TestModifyModel::initialized = false;

}
