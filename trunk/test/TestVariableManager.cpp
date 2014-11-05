#include <cppunit/ui/text/TestRunner.h>

#include "util/Unused.hpp"
#include "TestVariableManager.hpp"

using namespace std;

int main(int UNUSED(argc), char** UNUSED(argv)) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestVariableManager::GetTestSuite());
	bool failed = runner.run();
	return !failed;
}