/*
 * Logger.cpp
 *
 *  Created on: 11.01.2020
 *      Author: denis
 */

#include "Logger.hpp"
#include "Globals.hpp"

#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <sstream>
#include <thread>

using namespace sugo;
namespace logging = boost::log;
namespace expr    = logging::expressions;

Logger::Severity Logger::m_severity = Logger::DefaultSeverity;

void Logger::reinit(const std::string& instanceName)
{
    std::ostringstream        instanceId;
    static constexpr unsigned intHexSize = 8u;
    instanceId << instanceName << "@" << std::hex << std::setw(intHexSize) << std::setfill('0')
               << std::this_thread::get_id();

    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("Scope", logging::attributes::named_scope());
    logging::core::get()->set_filter(logging::trivial::severity >= m_severity);
    logging::core::get()->add_thread_attribute(
        "InstanceID", logging::attributes::constant<std::string>(instanceId.str()));
    logging::core::get()->add_thread_attribute(
        "File", logging::attributes::mutable_constant<std::string>(""));
    logging::core::get()->add_thread_attribute("Line",
                                               logging::attributes::mutable_constant<int>(0));
    logging::core::get()->add_thread_attribute(
        "Function", logging::attributes::mutable_constant<std::string>(""));
}

void Logger::init(Severity severity, const std::string& instanceName)
{
    m_severity = severity;
    reinit(instanceName);

    // console sink
    auto                      consoleSink  = logging::add_console_log(std::clog);
    static constexpr unsigned sizeSeverity = 5u;
    consoleSink->set_formatter(
        expr::stream << "["
                     << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
                                                                         "%Y-%m-%d %H:%M:%S.%f")
                     << "] [" << std::setw(sizeSeverity)
                     << expr::attr<logging::trivial::severity_level>("Severity") << "] "
                     << expr::smessage << " [" << expr::attr<std::string>("File") << ":"
                     << expr::attr<int>("Line") << ":" << expr::attr<std::string>("Function")
                     << "] [" << expr::attr<std::string>("InstanceID") << "]");
    consoleSink->locked_backend()->auto_flush(true);

    // fs sink
    //    auto fsSink = logging::add_file_log(
    //        logging::keywords::file_name = "test_%Y-%m-%d_%H-%M-%S.%N.log",
    //        logging::keywords::rotation_size = 10 * 1024 * 1024,
    //        logging::keywords::min_free_space = 30 * 1024 * 1024,
    //        logging::keywords::open_mode = std::ios_base::app);
    //    fsSink->set_formatter(logFmt);
    //    fsSink->locked_backend()->auto_flush(true);
}
