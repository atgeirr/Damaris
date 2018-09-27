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


using namespace logging::trivial;

namespace damaris
{

// this should be the only time we use a boost::shared_ptr
static boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> pLogSink;

// We should always make sure that the serverity_level enum has the same items as LogLevel enum, othewise
// the below casts are not valid anymore.
void EventLogger::Init(int processId , const std::string& file_name , int rotation_size , const std::string& log_format , int log_level)
{
    std::stringstream logFileName;
    logFileName << file_name << "_P" << processId << "_%N.log";

    pLogSink = logging::add_file_log(
                    keywords::file_name = logFileName.str(),                                           /*< file name pattern >*/
                    keywords::rotation_size = rotation_size * 1024 * 1024,                             /*< rotate files every 10 MiB... >*/
                   // keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),    /*< ...or at midnight >*/
                    keywords::format = log_format, //"[%TimeStamp%]: %Message%"                         /*< log record format >*/
                    keywords::open_mode = ( std::ios::out | std::ios::app)
            );

    logging::core::get()->set_filter(logging::trivial::severity >= (logging::trivial::severity_level)log_level);

    logging::add_common_attributes();
}

void EventLogger::Log(const std::string& message , EventLogger::LogLevel logLevel) {

    src::severity_logger <severity_level> lg;
    logging::trivial::severity_level severity = (logging::trivial::severity_level)logLevel;

    BOOST_LOG_SEV(lg, severity) << message;
}

void EventLogger::Flush() {
    if(pLogSink) {
        pLogSink->flush();
    }
}

} //namespace damaris
