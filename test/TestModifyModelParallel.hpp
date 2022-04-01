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


class TestModifyModelParallel : public CppUnit::TestFixture {
    
private:
    static bool initialized;
    int rank, size;
public:
    TestModifyModelParallel() {
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        /*if(not initialized) {
            Environment::Init("test.xml",MPI_COMM_WORLD);
            initialized = true;
        }*/
    }

    virtual ~TestModifyModelParallel() {
        //Environment::Finalize();
        //initialized = false;
    }

    static CppUnit::Test* GetTestSuite() {
        CppUnit::TestSuite *suiteOfTests =  new CppUnit::TestSuite("ModifyModel");
        
     
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModelParallel>(
                "Creates a ModifyModel class and preprocess using REGEX, then returns the model::simulation via the \
void * release from the unique_ptr<Simulation> ",
                &TestModifyModelParallel::CallCreateModifyModelandEnvViaVoid));
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModelParallel>(
                "Creates a ModifyModel class and preprocess using REGEX, then uses/modifies model::simulation directly \
from the unique_ptr<Simulation> before initializing the Environment object and testing",
                &TestModifyModelParallel::CallCreateModifyModelandEnvViaVoidModBySimObject));
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModelParallel>(
                "Reads the test.xml file and creates a Simulation model and then tests the XML has some valad values  \
e.g. servers per node == 1 and rank 3 is the dedicated core",
                &TestModifyModelParallel::CallReadXMLFromFileAndMakeModel));
        
         suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModelParallel>(
                "Reads the test.xml file and creates a Simulation model and then tests the XML has some valad values  \
e.g. servers per node == 2 and rank 2 and 3 are the dedicated core",
                &TestModifyModelParallel::CallReadXMLFromFileNotExist));
        
        
        return suiteOfTests;
    }

protected:
    
    
    void CallCreateModifyModelandEnvViaVoid() {
        // This creator is pre-defined with a Simulation XML string, 
        // however needs RepalceWithRegEx() to be valid
        damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived();  
        // std::cout << "Input string: " << std::endl  << myMod.GetConfigString()  << std::endl ;
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","1"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir/outputsubdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_",""},  // "store="MyStore"
        };
        myMod.RepalceWithRegEx(find_replace_map);
        
        // Print the regex'ed string
        // Check for outputdir/outputsubdir containing correct forward slash
        if (rank == 0) std::cout << myMod.GetConfigString() << std::endl ;
        
        
        myMod.SetSimulationModel() ;   
        
        Environment::Init(myMod.PassModelAsVoidPtr() ,MPI_COMM_WORLD);
        
        

        //unsigned long int buffer_size = static_cast<unsigned long>(mdl->architecture().buffer().size()) ;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing Environment::ServersPerNode ", 1, Environment::ServersPerNode() ) ;
        if (rank == 3) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing Environment::IsDedicatedCore() assuming: mpirun -np 4 and rank == 3 ", true, Environment::IsDedicatedCore() ) ;
        }
       
        Environment::Finalize();
        initialized = false;     
    }
    
    void CallCreateModifyModelandEnvViaVoidModBySimObject() {
        // This creator is pre-defined with a Simulation XML string, 
        // however needs RepalceWithRegEx() to be valid
        damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived();  
        // std::cout << "Input string: " << std::endl  << myMod.GetConfigString()  << std::endl ;
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","1"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_",""},
        };
        myMod.RepalceWithRegEx(find_replace_map);
        myMod.SetSimulationModel() ;   
        
        // Now we have the Simulation object we can modify it via the C++ API
        // if (rank == 0) std::cout << "Rank 0: myMod.GetModel()->architecture().buffer().size(): " << std::endl  << myMod.GetModel()->architecture().buffer().size() << std::endl ;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing REGEXed myMod.GetModel()->architecture().buffer().size()", 67108864ul, 
                                                  static_cast<unsigned long>(myMod.GetModel()->architecture().buffer().size() )) ;
        
        // Now modify the number of dedicated cores
        myMod.GetModel()->architecture().dedicated().cores( 2u )  ;
        
        Environment::Init(myMod.PassModelAsVoidPtr() ,MPI_COMM_WORLD);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing Environment::ServersPerNode ", 2, Environment::ServersPerNode() ) ;        
        CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( Environment::ServersPerNode() == 1) ) ;
        if (rank == 3) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing Environment::IsDedicatedCore() assuming: mpirun -np 4 and rank == 3 ", true, Environment::IsDedicatedCore() ) ;
        }
        if (rank == 2) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing Environment::IsDedicatedCore() assuming: mpirun -np 4 and rank == 2 ", true, Environment::IsDedicatedCore() ) ;
        }
       
        Environment::Finalize();
        initialized = false;    
    }
    
    
     void CallReadXMLFromFileAndMakeModel() {
         // This creator is pre-defined with a Simulation XML string, 
        // however needs RepalceWithRegEx() to be valid
        damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived();  
        
        
        myMod.ReadAndBroadcastXMLFromFile(MPI_COMM_WORLD, "test_modify.xml");
        
        // test_modify.xml should be able to be loaded by damaris without HDF, Paraview or Visit support
        myMod.SetSimulationModel() ;   
        
        Environment::Init(myMod.PassModelAsVoidPtr() ,MPI_COMM_WORLD);

        //unsigned long int buffer_size = static_cast<unsigned long>(mdl->architecture().buffer().size()) ;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing (from file test.xml) Environment::ServersPerNode ", 1, Environment::ServersPerNode() ) ;
        if (rank == 3) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing (from file test.xml) Environment::IsDedicatedCore() assuming: mpirun -np 4 and rank == 3 ", true, Environment::IsDedicatedCore() ) ;
        }
       
        Environment::Finalize();
        initialized = false;     
    }
    
     void CallReadXMLFromFileNotExist() {
         // This creator is pre-defined with a Simulation XML string, 
        // however needs RepalceWithRegEx() to be valid
        damaris::model::ModifyModelDerived myMod = damaris::model::ModifyModelDerived();  
        
        bool retbool = myMod.ReadAndBroadcastXMLFromFile(MPI_COMM_WORLD, "test_foo.xml");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Comparing (from file test_foo.xml) return value of ReadAndBroadcastXMLFromFile after trying to read file that does not exist", false, retbool ) ;
      
    }
    
    
    
    
};

bool TestModifyModelParallel::initialized = false;

}
