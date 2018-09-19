/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/


#ifndef __DAMARIS_EVENTLOGGER_H
#define __DAMARIS_EVENTLOGGER_H

// NOTE: these lines ignore lots of gcc wanrings caused by including common_attributes.hpp from boost library
#pragma GCC diagnostic push      // Save the current warning state
#pragma GCC diagnostic ignored "-Wmissing-field-initializers" // Disable the warning you're getting

#include <boost/log/utility/setup/common_attributes.hpp>

#pragma GCC diagnostic pop


#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Pointers.hpp"



namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;



namespace damaris {

/**
 * The EventLogger class acts as a wrapper class for Boost.Log library. The logging mechanism in Damaris can be easily changed in future
 * just by changing this class.
 */
class EventLogger {

    friend class Deleter<EventLogger>;

protected:
    /**
    * Constructor.
    */
    EventLogger() {}

    /**
     * Destructor.
     */
    virtual ~EventLogger() {}

public:

    enum LogLevel{Trace, Debug, Info, Warning, Error, Fatal};

    /**
    * creates the singleton object.
    */
    static std::shared_ptr<EventLogger> New()
    {
        std::shared_ptr<EventLogger> c(new EventLogger() , Deleter<EventLogger>());

        return c;
    }

	void Init(int processId , const std::string& file_name , int rotation_size , const std::string& log_format , int log_level);

    /**
    * Logs the message basaed on the logLevel severity mode
    */
	void Log(const std::string& message , EventLogger::LogLevel logLevel);

    /**
     * Forces the log file to be flushed.
     */
    void Flush();
};

}
#endif //__DAMARIS_EVENTLOGGER_H







