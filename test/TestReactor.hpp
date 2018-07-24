#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <mpi.h>

#include "util/Unused.hpp"
#include "comm/Channel.hpp"
#include "comm/Reactor.hpp"

namespace damaris {


class TestReactor : public CppUnit::TestFixture {
	
private:
	static std::shared_ptr<Reactor> reactor_;
	static int rank;
	static int callcheck;

	static void CallbackTest(int UNUSED(tag), int UNUSED(src), const void* buf, int UNUSED(size)) {
		for(int i=0; i<26; i++) {
			CPPUNIT_ASSERT(((const char*)buf)[i] == 'A'+i);
		}
		CPPUNIT_ASSERT(((const char*)buf)[26] == '\0');
		callcheck = 1;
		reactor_->Stop();
	}
	
	static void SyncTest(int UNUSED(tag), int UNUSED(src), const void* UNUSED(buf), int UNUSED(size)) {
		callcheck = 1;
		reactor_->Stop();
	}

public:
	TestReactor() {
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	}

	virtual ~TestReactor() {}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("Reactor");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestReactor>(
				"CreateReactor",
				&TestReactor::CreateReactor ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestReactor>(
				"Dispatch",
				&TestReactor::Dispatch ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestReactor>(
				"Broadcast",
				&TestReactor::Broadcast ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestReactor>(
				"Sync",
				&TestReactor::Sync ));
		
		return suiteOfTests;
	}

protected:
	void CreateReactor() {
		reactor_ = Reactor::New(MPI_COMM_WORLD);
		CPPUNIT_ASSERT(reactor_);
	}
	
	void Dispatch() {
		CPPUNIT_ASSERT(reactor_);
		CPPUNIT_ASSERT(reactor_->Bind(42,CallbackTest));
		
		if(rank == 2) {
			char* buf = new char[27];
			for(int i=0; i<26; i++) buf[i] = 'A'+i;
			buf[26] = '\0';
			CPPUNIT_ASSERT(reactor_->Dispatch(42,3,buf,27));
			delete[] buf;
		}
		
		if(rank == 3) {
			reactor_->Run();
			CPPUNIT_ASSERT(callcheck == 1);
			callcheck = 0;
		}
	}
	
	void Broadcast() {
		CPPUNIT_ASSERT(reactor_);
		CPPUNIT_ASSERT(reactor_->Bind(42,CallbackTest));
		char* buf = new char[27];
		
		if(rank == 2) {
			
			for(int i=0; i<26; i++) buf[i] = 'A'+i;
			buf[26] = '\0';
			CPPUNIT_ASSERT(reactor_->Broadcast(42,buf,27));
			
		}

		std::cout << "XXXXXXXXXX" << std::endl;
		while(callcheck != 1) reactor_->PollOne();
		std::cout << "YYYYYYYYYY" << std::endl;
		CPPUNIT_ASSERT(callcheck == 1);
		if(buf) delete[] buf;
		callcheck = 0;
	}
	
	void Sync() {
		CPPUNIT_ASSERT(reactor_);
		CPPUNIT_ASSERT(reactor_->Bind(43,SyncTest));
		CPPUNIT_ASSERT(reactor_->Sync(43));
	
		reactor_->Run();

		CPPUNIT_ASSERT(callcheck == 1);
		callcheck = 0;
	}
};

std::shared_ptr<Reactor> TestReactor::reactor_;
int TestReactor::rank;
int TestReactor::callcheck = 0;

}
