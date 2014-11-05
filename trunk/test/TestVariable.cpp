#include <cppunit/ui/text/TestRunner.h>
#include <mpi.h>

#include "TestVariable.hpp"

using namespace std;

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestVariable::GetTestSuite());
	bool failed = runner.run();
	MPI_Finalize();
	return !failed;
}