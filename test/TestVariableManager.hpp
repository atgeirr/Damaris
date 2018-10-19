#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <mpi.h>

#include "damaris/model/BcastXML.hpp"
#include "damaris/data/VariableManager.hpp"
#include "damaris/data/LayoutManager.hpp"
#include "damaris/data/ParameterManager.hpp"

namespace damaris {


class TestVariableManager : public CppUnit::TestFixture {
	
private:
	static std::shared_ptr<model::Simulation> mdl;
public:
	TestVariableManager() {
		if(not mdl) {
			mdl = model::LoadXML("test.xml");
		}
	}

	virtual ~TestVariableManager() {
		
	}

	static CppUnit::Test* GetTestSuite() {
		
		
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("VariableManager");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestVariableManager>(
				"VariableManager initialization",
				&TestVariableManager::InitVariableManager ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestVariableManager>(
				"Search for some variables",
				&TestVariableManager::SearchVariable ));

		return suiteOfTests;
	}

protected:
	void InitVariableManager() {
		CPPUNIT_ASSERT(VariableManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(VariableManager::IsEmpty());
		VariableManager::Init(mdl->data());
		CPPUNIT_ASSERT(not VariableManager::IsEmpty());
		CPPUNIT_ASSERT(VariableManager::GetNumObjects() == 3);
		VariableManager::DeleteAll();
		CPPUNIT_ASSERT(VariableManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(VariableManager::IsEmpty());
	}
	
	void SearchVariable() {
		// initialization
		VariableManager::Init(mdl->data());
		// search an existing variable
		std::shared_ptr<Variable> v1 
			= VariableManager::Search("coordinates/x2d");
		// search a non-existing variable
		std::shared_ptr<Variable> v2
			= VariableManager::Search("something/undefined");
		// the first variable should be valid
		CPPUNIT_ASSERT((bool)v1);
		// the second should not be valid
		CPPUNIT_ASSERT(not (bool)v2);
		// delete everything
		VariableManager::DeleteAll();
		// the number of objects should go down to 0
		CPPUNIT_ASSERT(VariableManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(VariableManager::IsEmpty());
		// the instance v1 should still exist thanks to std::shared_ptr
		CPPUNIT_ASSERT((bool)v1);
		// now search again for v1
		v1 = VariableManager::Search("coordinates/x2d");
		// it should not exist
		CPPUNIT_ASSERT(not (bool)v1);
	}
};

std::shared_ptr<model::Simulation> TestVariableManager::mdl;

}
