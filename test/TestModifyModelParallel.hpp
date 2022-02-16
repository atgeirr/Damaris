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

extern "C" void my_function(const char* name, int32_t source, int32_t /*iteration*/,
		const char* args)
{
	CPPUNIT_ASSERT(std::string(name) == "test_event");
	CPPUNIT_ASSERT(source == 1);
	std::cout << "hello world !" << std::endl;
}

namespace damaris {


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
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("ModifyModel");
		
    
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestModifyModelParallel>(
				"Creates a ModifyModel class and preprocess using REGEX, then returns the model::simulation via the void * release from the unique_ptr<Simulation> ",
				&TestModifyModelParallel::CallCreateModifyModelandEnvViaVoid));
		
		return suiteOfTests;
	}

protected:
	
	
	void CallCreateModifyModelandEnvViaVoid() {
		damaris::model::ModifyModel myMod = damaris::model::ModifyModel();  // this creator is pre-defined with a Simulation XML string
        // std::cout << "Input string: " << std::endl  << myMod.getConfigString()  << std::endl ;
        std::map<std::string,std::string> find_replace_map = {
        {"_SHMEM_BUFFER_BYTES_REGEX_","67108864"},
        {"_DC_REGEX_","1"},
        {"_DN_REGEX_","0"},
        {"_PATH_REGEX_","outputdir"},
        {"_MYSTORE_OR_EMPTY_REGEX_","MyStore"},
        };
        myMod.RepalceWithRegEx(find_replace_map);
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
	
};

bool TestModifyModelParallel::initialized = false;

}
