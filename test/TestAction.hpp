#include <iostream>
#include <set>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "damaris/model/Model.hpp"
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


class TestAction : public CppUnit::TestFixture {
    
private:
    static bool initialized;
public:
    TestAction() {
        if(not initialized) {
            Environment::Init("test.xml",MPI_COMM_WORLD);
            initialized = true;
        }
    }

    virtual ~TestAction() {
        Environment::Finalize();
        initialized = false;
    }

    static CppUnit::Test* GetTestSuite() {
        CppUnit::TestSuite *suiteOfTests = 
            new CppUnit::TestSuite("Action");
        
        suiteOfTests->addTest(new CppUnit::TestCaller<TestAction>(
                "Calls an action",
                &TestAction::CallAction));
        
        return suiteOfTests;
    }

protected:
    
    void CallAction() {
        if(Environment::IsClient()) {
            std::shared_ptr<Action> a 
                = ActionManager::Search("test_event");
            CPPUNIT_ASSERT(a);
            a->Call(1,2,NULL);
        }
    }
};

bool TestAction::initialized = false;

}
