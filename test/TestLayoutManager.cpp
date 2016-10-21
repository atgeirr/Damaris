#include <cppunit/ui/text/TestRunner.h>

#include "TestLayoutManager.hpp"

using namespace std;

int main() {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(damaris::TestLayoutManager::GetTestSuite());
	bool failed = runner.run();
	return !failed;
}