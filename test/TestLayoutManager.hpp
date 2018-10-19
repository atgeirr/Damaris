#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "damaris/model/BcastXML.hpp"
#include "damaris/data/LayoutManager.hpp"

namespace damaris {


class TestLayoutManager : public CppUnit::TestFixture {
	
private:
	static std::shared_ptr<model::Simulation> mdl;
	
public:
	TestLayoutManager() {
		if(not mdl) {
			mdl = model::LoadXML("test.xml");
		}
	}

	virtual ~TestLayoutManager() {
		
	}

	static CppUnit::Test* GetTestSuite() {
		
		
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("LayoutManager");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestLayoutManager>(
				"LayoutManager initialization",
				&TestLayoutManager::InitLayoutManager ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestLayoutManager>(
				"Search for some layouts",
				&TestLayoutManager::SearchLayout ));

		return suiteOfTests;
	}

protected:
	void InitLayoutManager() {
		// before initialization, number of objects is 0
		CPPUNIT_ASSERT(LayoutManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(LayoutManager::IsEmpty());
		// initialization
		LayoutManager::Init(mdl->data());
		// LayoutManager should not be empty
		CPPUNIT_ASSERT(not LayoutManager::IsEmpty());
		// the number of type-based layout is defined by
		// the last enum value (model::Type::undefined)
		int n = model::Type::undefined;
		CPPUNIT_ASSERT(LayoutManager::GetNumObjects() == (n + 3));
		// delete everything
		LayoutManager::DeleteAll();
		CPPUNIT_ASSERT(LayoutManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(LayoutManager::IsEmpty());
	}
	
	void SearchLayout() {
		// initialization
		LayoutManager::Init(mdl->data());
		// search for an existing layout
		std::shared_ptr<Layout> l1
			= LayoutManager::Search("rmeshY");
		// search for a non-existing layout
		std::shared_ptr<Layout> l2
			= LayoutManager::Search("something");
		// search for a type layout
		std::shared_ptr<Layout> l3
			= LayoutManager::Search("integer");
		// l1 should exist, l2 should not, l3 should
		CPPUNIT_ASSERT((bool)l1);
		CPPUNIT_ASSERT(not (bool)l2);
		CPPUNIT_ASSERT((bool)l3);
		// delete everything
		LayoutManager::DeleteAll();
		CPPUNIT_ASSERT(LayoutManager::GetNumObjects() == 0);
		CPPUNIT_ASSERT(LayoutManager::IsEmpty());
		l1 = LayoutManager::Search("rmeshY");
		CPPUNIT_ASSERT(not (bool)l1);
	}
};

std::shared_ptr<model::Simulation> TestLayoutManager::mdl;

}
