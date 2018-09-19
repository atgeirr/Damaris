#include <iostream>
#include <cstdio>
#include <string>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include "log/EventLogger.hpp"


namespace damaris {


class TestEventLogger : public CppUnit::TestFixture {
	
private:
	std::shared_ptr<EventLogger> eventLogger_;
    std::string fileName;
    std::string logFileName;

    std::string traceMessage;
    std::string debugMessage;
    std::string infoMessage;
    std::string warningMessage;
    std::string errorMessage;
    std::string fatalMessage;

	inline bool FileExists(const std::string& file_name) {
        std::ifstream f(file_name.c_str());
		return f.good();
	}

	inline bool WordExistsInFile(const std::string& file_name , const std::string& word) {

        std::string line;
        std::ifstream read_file;

		if (!FileExists(file_name)) return false;

		read_file.open(file_name.c_str());

		while( getline(read_file, line) )
		{
			if (line.find(word) != std::string::npos)
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

		fileName = "/tmp/TestEventLogger";
		logFileName = "/tmp/TestEventLogger_P2_0.log";
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


		eventLogger_->Init(2 /*rank*/,
						   fileName /*log file*/,
						   1 /*rotation_size - MB*/ ,
						   "[%TimeStamp%]: %Message%" /*Msg Format*/,
						   2 /*Log Level - Info*/);
	}

	void Log() {
		eventLogger_->Log(traceMessage, damaris::EventLogger::Trace);
		eventLogger_->Log(debugMessage, damaris::EventLogger::Debug);
		eventLogger_->Log(infoMessage,  damaris::EventLogger::Info);
		eventLogger_->Log(warningMessage, damaris::EventLogger::Warning);
		eventLogger_->Log(errorMessage, damaris::EventLogger::Error);
		eventLogger_->Log(fatalMessage, damaris::EventLogger::Fatal);
        eventLogger_->Flush();
    }

	void CheckLog() {
		CPPUNIT_ASSERT(FileExists(logFileName));

		CPPUNIT_ASSERT(WordExistsInFile(logFileName , traceMessage) == false);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , debugMessage) == false);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , infoMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , warningMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , errorMessage) == true);
		CPPUNIT_ASSERT(WordExistsInFile(logFileName , fatalMessage) == true);
        remove(logFileName.c_str());
    }
};


}
