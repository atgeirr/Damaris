#include <iostream>
#include <set>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "model/Model.hpp"
#include "model/BcastXML.hpp"
#include "data/VariableManager.hpp"
#include "env/Environment.hpp"

namespace damaris {


class TestVariable : public CppUnit::TestFixture {
	
private:
	static bool initialized;
public:
	TestVariable() {
		if(not initialized) {
			Environment::Init("test.xml",MPI_COMM_WORLD);
			initialized = true;
		}
	}

	virtual ~TestVariable() {
		Environment::Finalize();
		initialized = false;
	}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("Variable");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestVariable>(
				"Allocate/Remove blocks",
				&TestVariable::AllocateBlock));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestVariable>(
				"Search existing blocks",
				&TestVariable::SearchBlock));
		
		return suiteOfTests;
	}

protected:
	
	void AllocateBlock() {
		int rank;
		MPI_Comm c = Environment::GetEntityComm();
		if(Environment::IsClient()) {
			MPI_Comm_rank(c,&rank);
			std::shared_ptr<Variable> v 
				= VariableManager::Search("coordinates/x2d");
			CPPUNIT_ASSERT(v);
			std::shared_ptr<Block> b = v->Allocate(rank,0,0,false);
			CPPUNIT_ASSERT(b);
			//v->DetachBlock(b);
		}
	}
	
	void SearchBlock() {
		int rank;
		MPI_Comm c = Environment::GetEntityComm();
		if(Environment::IsClient()) {
			MPI_Comm_rank(c,&rank);
			std::shared_ptr<Variable> v 
				= VariableManager::Search("coordinates/x2d");
			CPPUNIT_ASSERT(v);
			std::shared_ptr<Block> b = v->GetBlock(rank,1,0);
			CPPUNIT_ASSERT(not b);
			b = v->GetBlock(rank,0,0);
			CPPUNIT_ASSERT(b);
		}
	}
};

bool TestVariable::initialized = false;

}
