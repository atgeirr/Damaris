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

using namespace std;
	
USING_POINTERS;

class TestChannel : public CppUnit::TestFixture {
	
private:
	static shared_ptr<Reactor> reactor_;
	static shared_ptr<Channel> channel_;
	static int rank;

	static void CallbackTest(int UNUSED(tag), int UNUSED(src), const void* buf, int UNUSED(size)) {
		for(int i=0; i<26; i++) {
			CPPUNIT_ASSERT(((const char*)buf)[i] == 'A'+i);
		}
		CPPUNIT_ASSERT(((const char*)buf)[26] == '\0');
		reactor_->Stop();
	}

public:
	TestChannel() {
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	}

	virtual ~TestChannel() {}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("Channel");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestChannel>(
				"CreateChannel",
				&TestChannel::CreateChannel ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestChannel>(
				"BlockingComm",
				&TestChannel::BlockingComm ));

		suiteOfTests->addTest(new CppUnit::TestCaller<TestChannel>(
				"AsyncComm",
				&TestChannel::AsyncComm ));
		
		return suiteOfTests;
	}

protected:
	void CreateChannel() {
		channel_ = Channel::New(reactor_,
				MPI_COMM_WORLD,rank == 0 ? 1 : 0, 32);
		CPPUNIT_ASSERT(not channel_);
		
		reactor_ = Reactor::New(MPI_COMM_SELF);
		
		channel_ = Channel::New(reactor_,
				MPI_COMM_WORLD,rank == 0 ? 1 : 0, 32);
		CPPUNIT_ASSERT(channel_);
	}
	
	void BlockingComm() {
		CPPUNIT_ASSERT(channel_);
		
		char* buf = new char[27];
		
		if(rank == 0) {
			for(int i=0; i<26; i++) buf[i] = 'A'+i;
			buf[26] = '\0';
			
			channel_->Send(0,buf,27);
		} else {
			
			channel_->Recv(0,buf,27);
			for(int i=0; i<26; i++) {
				CPPUNIT_ASSERT(buf[i] == 'A'+i);
			}
			CPPUNIT_ASSERT(buf[26] == '\0');
		}
		MPI_Barrier(MPI_COMM_WORLD);	
		delete[] buf;
	}
	
	void AsyncComm() {
		CPPUNIT_ASSERT(channel_);
		
		char* buf = new char[27];
		
		if(rank == 0) {
			for(int i=0; i<26; i++) buf[i] = 'A'+i;
			buf[26] = '\0';
			
			//CPPUNIT_ASSERT(
			int err = channel_->AsyncSend(0,buf,27,CallbackTest);
				//== DAMARIS_OK);
			std::cout << "error code is " << err << "\n";
		} else {
			CPPUNIT_ASSERT(
				channel_->AsyncRecv(0,buf,27,CallbackTest)
				== DAMARIS_OK);
		}
		
		reactor_->Poll();
		
		delete[] buf;
	}
};

shared_ptr<Reactor> TestChannel::reactor_;
shared_ptr<Channel> TestChannel::channel_;
int TestChannel::rank;

}
