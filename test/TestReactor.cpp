#include <cppunit/ui/text/TestRunner.h>
#include <iostream>

#include <mpi.h>

#include "TestReactor.hpp"

using namespace std;

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(size != 4) {
		std::cerr << "This test should be run using MPI on 4 processes"
			<< std::endl;
		return 1;
	}
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestReactor::GetTestSuite());
	bool failed = runner.run();
	MPI_Finalize();
	return !failed;
}