#include <cppunit/ui/text/TestRunner.h>
#include <iostream>


#include "TestEventLogger.hpp"

using namespace std;

int main(int argc, char** argv) {

	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestEventLogger::GetTestSuite());
	bool failed = runner.run();

	return !failed;
}