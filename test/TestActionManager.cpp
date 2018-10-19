#include <cppunit/ui/text/TestRunner.h>

#include "damaris/util/Unused.hpp"
#include "TestActionManager.hpp"

using namespace std;

int main(int UNUSED(argc), char** UNUSED(argv)) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestActionManager::GetTestSuite());
	bool failed = runner.run();
	return !failed;
}
