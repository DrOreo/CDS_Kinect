#pragma once

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/attributes/named_scope.hpp>


#include <boost/log/trivial.hpp>  

namespace Blogging = boost::log;
namespace Battrs = boost::log::attributes;
namespace Bsrc = boost::log::sources;
namespace Bsinks = boost::log::sinks;
namespace Bexpr = boost::log::expressions;
namespace Bkeywords = boost::log::keywords;

enum SeverityLevel
{
	Log_Info,
	Log_Notice,
	Log_Debug,
	Log_Warning,
	Log_Error,
	Log_Fatal
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
	std::basic_ostream< CharT, TraitsT >& strm, SeverityLevel lvl)
{
	static const char* const str[] =
	{
		"Info",
		"Notice",
		"Debug",
		"Warning",
		"Error",
		"Fatal"
	};
	if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
		strm << str[lvl];
	else
		strm << static_cast< int >(lvl);
	return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(log_severity, "Severity", SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_uptime, "Uptime", Battrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_scope, "Scope", Battrs::named_scope::value_type)

void g_InitLog();

