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

#include <sstream>
#include "log/EventLogger.hpp"

using namespace std;
using namespace logging::trivial;

namespace damaris
{

void EventLogger::Init(int processId , string file_name , int rotation_size , string log_format , int log_level)
{
    stringstream logFileName;
    logFileName << file_name << "_P" << processId << "_%N.log";

    logging::add_file_log(
                    keywords::file_name = logFileName.str(),                                           /*< file name pattern >*/
                    keywords::rotation_size = rotation_size * 1024 * 1024,                             /*< rotate files every 10 MiB... >*/
                   // keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),    /*< ...or at midnight >*/
                    keywords::format = log_format, //"[%TimeStamp%]: %Message%"                         /*< log record format >*/
                    keywords::open_mode = ( std::ios::out | std::ios::app)
            );

    logging::core::get()->set_filter(logging::trivial::severity >= (logging::trivial::severity_level)log_level);

    logging::add_common_attributes();
}

void EventLogger::LogTrace(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, trace) << message;
}

void EventLogger::LogDebug(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, debug) << message;
}

void EventLogger::LogInfo(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, info) << message;
}

void EventLogger::LogWarning(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, warning) << message;
}

void EventLogger::LogError(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, error) << message;
}

void EventLogger::LogFatal(string message)
{
    src::severity_logger <severity_level> lg;

    BOOST_LOG_SEV(lg, fatal) << message;
}

} //namespace damaris