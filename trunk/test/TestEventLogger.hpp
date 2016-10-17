#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

//#include <mpi.h>

//#include "util/Unused.hpp"
//#include "comm/Channel.hpp"
//#include "comm/Reactor.hpp"
#include "log/EventLogger.hpp"


namespace damaris {

using namespace std;
	
USING_POINTERS;

class TestEventLogger : public CppUnit::TestFixture {
	
private:
	shared_ptr<EventLogger> eventLogger_;
	string fileName;
	string logFileName;

	string traceMessage;
	string debugMessage;
	string infoMessage;
	string warningMessage;
	string errorMessage;
	string fatalMessage;

	inline bool FileExists(const string& file_name) {
		ifstream f(file_name.c_str());
		return f.good();
	}

	inline bool WordExistsInFile(const string& file_name , const string& word) {

		string line;
		ifstream read_file;

		if (FileExists(file_name.c_str()) == false) return false;

		read_file.open(file_name.c_str());

		while( getline(read_file, line) )
		{
			if (line.find(word) != string::npos)
				return true;
		}
		return false;
	}

public:
	TestEventLogger() {
		eventLogger_ = EventLogger::New();

		traceMessage = "Trace Log Message";
		debugMessage = "Debug Log Message";
		infoMessage = "Info Log Message";
		warningMessage = "Warning Log Message";
		errorMessage = "Error Log Message";
		fatalMessage = "Fatal Log Message";
	}

	virtual ~TestEventLogger() {}

	static CppUnit::Test* GetTestSuite() {
		CppUnit::TestSuite *suiteOfTests = 
			new CppUnit::TestSuite("EventLogger");
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestEventLogger>(
				"InitiateEventLogger",
				&TestEventLogger::InitiateEventLogger ));
		
		suiteOfTests->addTest(new CppUnit::TestCaller<TestEventLogger>(
				"Log",
				&TestEventLogger::Log ));

		suiteOfTests->addTest(new CppUnit::TestCaller<TestEventLogger>(
				"CheckLog",
				&TestEventLogger::CheckLog ));

		return suiteOfTests;
	}

protected:
	void InitiateEventLogger() {

		fileName = "./TestEventLogger";
		logFileName = "./TestEventLogger_P2_0.log";

		eventLogger_->Init(2 /*rank*/,
						   fileName /*log file*/,
						   1 /*rotation_size - MB*/ ,
						   "[%TimeStamp%]: %Message%" /*Msg Format*/,
						   1 /*Log Level - Info*/);
	}

	void Log() {
		eventLogger_->LogTrace(traceMessage);
		eventLogger_->LogDebug(debugMessage);
		eventLogger_->LogInfo(infoMessage);
		eventLogger_->LogWarning(warningMessage);
		eventLogger_->LogError(errorMessage);
		eventLogger_->LogFatal(fatalMessage);
	}

	void CheckLog() {
		FileExists(logFileName);

		CPPUNIT_ASSERT(WordExistsInFile(logFileName , traceMessage) == false);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , debugMessage) == false);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , infoMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , warningMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , errorMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , fatalMessage) == true);
	}
};


}
