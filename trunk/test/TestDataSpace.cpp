#include <cppunit/ui/text/TestRunner.h>

#include "TestDataSpace.hpp"

using namespace std;

int main() {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestDataSpace::GetTestSuite());
	bool failed = runner.run();
	return !failed;
}