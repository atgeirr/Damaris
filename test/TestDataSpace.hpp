#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "damaris/buffer/DataSpace.hpp"

namespace damaris {


class TestDataSpace : public CppUnit::TestFixture {
	
private:
	class TestOwner : public ENABLE_SHARED_FROM_THIS(TestOwner) {
	private:
		char data[256];
		
	public:
		
		bool LinkDataSpace(DataSpace<TestOwner>& ds)
		{
			return ds.Link(SHARED_FROM_THIS(),data,256);
		}
	
		bool Deallocate(void* p) 
		{
			if(p != NULL) {
				return true;
			} else {
				return false;
			}
		}
	};
	
public:
	TestDataSpace() {}

	virtual ~TestDataSpace() {}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("DataSpace");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestDataSpace>(
				"Non-initialized DataSpaces",
				&TestDataSpace::CreateNonLinkedDataSpaces ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestDataSpace>(
				"Linking one DataSpace",
				&TestDataSpace::LinkOneDataSpace ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestDataSpace>(
				"Copy DataSpaces",
				&TestDataSpace::CopyDataSpaces ));

		return suiteOfTests;
	}

protected:
	void CreateNonLinkedDataSpaces() {
		DataSpace<TestOwner> ds1;
		CPPUNIT_ASSERT(not ds1.IsLinked());
		CPPUNIT_ASSERT(ds1.RefCount() == 0);
		DataSpace<TestOwner> ds2 = ds1;
		CPPUNIT_ASSERT(not ds2.IsLinked());
		CPPUNIT_ASSERT(ds2.RefCount() == 0);
	}
	
	void LinkOneDataSpace() {
		std::shared_ptr<TestOwner> owner(new TestOwner);
		{
			DataSpace<TestOwner> ds;
			owner->LinkDataSpace(ds);
			CPPUNIT_ASSERT(ds.IsLinked());
			CPPUNIT_ASSERT(ds.RefCount() == 1);
			ds.Unlink();
			CPPUNIT_ASSERT(not ds.IsLinked());
			CPPUNIT_ASSERT(ds.RefCount() == 0);
		}
	}
	
	void CopyDataSpaces() {
		std::shared_ptr<TestOwner> owner(new TestOwner);
		{
			DataSpace<TestOwner> ds1;
			owner->LinkDataSpace(ds1);
			{
				DataSpace<TestOwner> ds2 = ds1;
				CPPUNIT_ASSERT(ds2.IsLinked());
				CPPUNIT_ASSERT(ds1.RefCount() == 2);
				CPPUNIT_ASSERT(ds2.RefCount() == 2);
			}
			CPPUNIT_ASSERT(ds1.RefCount() == 1);
		}
	}
};

}
