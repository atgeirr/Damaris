#include <cppunit/ui/text/TestRunner.h>
#include <mpi.h>

#include "TestModifyModel.hpp"

using namespace std;

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestModifyModel::GetTestSuite());
	bool failed = runner.run();
	MPI_Finalize();
	return !failed;
}
