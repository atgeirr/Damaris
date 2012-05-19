/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file Debug.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 *
 * Debug.hpp contains all macros to print informations, error messages, etc.
 */

/* 
    ** DEBUGGING CONFIGURATION **
    - ERROR is controled externally: -D__ERROR
    - INFO assumes ERROR, define only -D__INFO
    - ASSERT is controled externally: -D__ASSERT
	- FATAL is not controled, it takes a condition and throws a runtime_error if the exception 
		is not satisfied.
	- TRACE is controled externally: -D__TRACE
    - DEBUG is controled locally (to allow selective debugging). Define __DEBUG before including 'Debug.hpp'.
*/

#ifndef __DEBUG_CONFIG
#define __DEBUG_CONFIG

#include <stdexcept>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>

#define __RFILE__ boost::filesystem::path(__FILE__).leaf()

#ifdef __BENCHMARK
#define TIMER_START(timer) boost::posix_time::ptime timer(boost::posix_time::microsec_clock::local_time());
#define TIMER_STOP(timer, message) {\
	boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());\
	boost::posix_time::time_duration t = now - timer;\
	std::cout << "[BENCHMARK " << now << "] [" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "] [time elapsed: " << t << " us] " << message << std::endl;\
    }
#else
#define TIMER_START(timer) boost::posix_time::ptime timer;
#define TIMER_STOP(timer, message)
#endif

#define MESSAGE(out, level, message)\
    out << "[" << level << " " << boost::posix_time::microsec_clock::local_time() << "] [" \
    << __RFILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "] " << message << std::endl

#define SIMPLE(out,level, message)\
	out << "[" << level << " " << boost::posix_time::microsec_clock::local_time() << "] "\
	<< message << std::endl

#ifdef __INFO
#define __ERROR
#define __WARN
#define __CFGERROR
#define __CFGWARN
#define INFO(message) MESSAGE(std::cout, "INFO", message)
#else
#define INFO(message)
#endif

#ifdef __TRACE
#define TRACE(out,message) SIMPLE(out, "TRACE", message)
#else
#define TRACE(out,message)
#endif

#ifdef __ERROR
#define ERROR(message) MESSAGE(std::cerr, "ERROR", message)
#else
#define ERROR(message)
#endif

#ifdef __CFGERROR
#define CFGERROR(message) MESSAGE(std::cerr, "CONFIG-ERROR",message)
#else
#define CFGERROR(message)
#endif

#ifdef __WARN
#define WARN(message) MESSAGE(std::cerr, "WARNING", message)
#else
#define WARN(message)
#endif

#ifdef __CFGWARN
#define CFGWARN(message) MESSAGE(std::cerr, "CONFIG-WARNING", message)
#else
#define CFGWARN(message)
#endif

#ifdef __ASSERT
#define ASSERT(expression) {\
	if (!(expression)) {\
	    std::ostringstream out;\
	    MESSAGE(out, "ASSERT", "failed on expression: " << #expression);\
	    throw std::runtime_error(out.str());\
	}\
    }
#else
#define ASSERT(expression)
#endif

#define FATAL(expression,msg) {\
	if(expression) {\
		std::ostringstream out; \
		MESSAGE(out, "FATAL", msg);\
		throw std::runtime_error(out.str()); \
	}\
	}

#ifdef __DEBUG_ALL
#define DBG(message) MESSAGE(std::cout, "DEBUG", message)
#else
#undef DBG
#ifdef __DEBUG
#define DBG(message) MESSAGE(std::cout, "DEBUG", message)
#undef __DEBUG
#else
#define DBG(message)
#endif
#endif

#endif