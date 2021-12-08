#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <mpi.h>

#include "damaris/model/BcastXML.hpp"
#include "damaris/action/ActionManager.hpp"

namespace damaris {


class TestActionManager : public CppUnit::TestFixture {
	
private:
	static std::shared_ptr<model::Simulation> mdl;
public:
	TestActionManager() {
		if(not mdl) {
			mdl = model::LoadXML("test.xml");
		}
	}

	virtual ~TestActionManager() {
		
	}

	static CppUnit::Test* GetTestSuite() {
		
		
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("ActionManager");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestActionManager>(
				"ActionManager initialization",
				&TestActionManager::InitActionManager ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestActionManager>(
				"Search for some action",
				&TestActionManager::SearchAction ));

		return suiteOfTests;
	}

protected:
	void InitActionManager() {
		CPPUNIT_ASSERT(ActionManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(ActionManager::IsEmpty());
		ActionManager::Init(mdl->actions().get());
		CPPUNIT_ASSERT(not ActionManager::IsEmpty());
		ActionManager::DeleteAll();
		CPPUNIT_ASSERT(ActionManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(ActionManager::IsEmpty());
	}
	
	void SearchAction() {
		// initialization
		ActionManager::Init(mdl->actions().get());
		// search an existing action
		std::shared_ptr<Action> a1 
			= ActionManager::Search("test_event");
		// search a non-existing action
		std::shared_ptr<Action> a2
			= ActionManager::Search("something_undefined");
		// the first action should be valid
		CPPUNIT_ASSERT((bool)a1);
		// the second should not be valid
		CPPUNIT_ASSERT(not (bool)a2);
		// delete everything
		ActionManager::DeleteAll();
		// the number of objects should go down to 0
		CPPUNIT_ASSERT(ActionManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(ActionManager::IsEmpty());
		// the instance a1 should still exist thanks to std::shared_ptr
		CPPUNIT_ASSERT((bool)a1);
		// now search again for a1
		a1 = ActionManager::Search("test_event");
		// it should not exist
		CPPUNIT_ASSERT(not (bool)a1);
	}
};

std::shared_ptr<model::Simulation> TestActionManager::mdl;

}
