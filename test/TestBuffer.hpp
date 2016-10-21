#include <iostream>
#include <list>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "model/Model.hpp"
#include "model/BcastXML.hpp"
#include "buffer/SharedMemorySegment.hpp"

namespace damaris {

using namespace std;
	
USING_POINTERS;

class TestBuffer : public CppUnit::TestFixture {
	
private:
	int rank, size;
	static shared_ptr<Buffer> buffer;
	static shared_ptr<model::Simulation> mdl;
public:
	TestBuffer() {
		MPI_Comm_size(MPI_COMM_WORLD,&size);
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
		if(not mdl) {
			mdl = model::LoadXML("test.xml");
		}
	}

	virtual ~TestBuffer() {}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("Buffer");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestBuffer>(
				"Create/Open Buffer",
				&TestBuffer::CreateOpenBuffer));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestBuffer>(
				"Read/Write on Buffer",
				&TestBuffer::ReadWriteBuffer));

		suiteOfTests->addTest(new CppUnit::TestCaller<TestBuffer>(
				"Remove Buffer",
				&TestBuffer::RemoveBuffer));
		
		return suiteOfTests;
	}

protected:
	void CreateOpenBuffer() {
		if(rank == 0) {
			buffer = SharedMemorySegment::Create(
				mdl->architecture().buffer());
			MPI_Barrier(MPI_COMM_WORLD);
			
		} else {
			MPI_Barrier(MPI_COMM_WORLD);
			buffer = SharedMemorySegment::Open(
				mdl->architecture().buffer());
		}
		CPPUNIT_ASSERT(buffer);
	}
	
	void ReadWriteBuffer() {
		MPI_Barrier(MPI_COMM_WORLD);
		CPPUNIT_ASSERT(buffer);
		if(rank == 0) {
			DataSpace<Buffer> ds;
			CPPUNIT_ASSERT(not buffer->PointerBelongsToSegment(ds.GetData()));
			ds = buffer->Allocate(256);
			CPPUNIT_ASSERT(ds.GetSize() == 256);
			char* b = (char*)ds.GetData();
			CPPUNIT_ASSERT(buffer->PointerBelongsToSegment(b));
			CPPUNIT_ASSERT(buffer->PointerBelongsToSegment(b+32));
			
			for(int i = 0; i<256; i++) {
				b[i] = i;
			}
			Handle h = buffer->GetHandleFromAddress((void*)b);
			MPI_Send(&h,sizeof(h),MPI_BYTE,1,0,MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
		} else 	{
			Handle h;
			MPI_Recv(&h,sizeof(h),MPI_BYTE,0,0,MPI_COMM_WORLD,
					MPI_STATUS_IGNORE);
			char* b = (char*)buffer->GetAddressFromHandle(h);
			CPPUNIT_ASSERT(b);
			for(int i = 0; i<256; i++) {
				CPPUNIT_ASSERT((unsigned char)(b[i]) == i);
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	
	void OverloadBuffer() {
		MPI_Barrier(MPI_COMM_WORLD);
		CPPUNIT_ASSERT(buffer);
		size_t initial_size;
		if(rank == 0) {
			size_t free_size = buffer->GetFreeMemory();
			initial_size = free_size;
			std::list<DataSpace<Buffer> > addresses;
			while(free_size >= 256) {
				DataSpace<Buffer> b = buffer->Allocate(256);
				CPPUNIT_ASSERT(b.GetSize() == 256);
				addresses.push_back(b);
				free_size = buffer->GetFreeMemory();
			}
			DataSpace<Buffer> b = buffer->Allocate(256);
			CPPUNIT_ASSERT(b.GetSize() == 0);
			addresses.clear();
			free_size = buffer->GetFreeMemory();
			CPPUNIT_ASSERT(free_size == initial_size);
		}
	}
	
	void RemoveBuffer() {
		MPI_Barrier(MPI_COMM_WORLD);
		CPPUNIT_ASSERT(buffer);
		if(rank == 0) {
			SharedMemorySegment::Remove(
				mdl->architecture().buffer());
		}
		buffer.reset();
		CPPUNIT_ASSERT(not buffer);
	}
};

shared_ptr<Buffer> TestBuffer::buffer;
shared_ptr<model::Simulation> TestBuffer::mdl;

}
