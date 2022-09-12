/*
 * Logger.hpp
 *
 *  Created on: 11.01.2020
 *      Author: denis
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

namespace sugo
{
namespace log
{
#define LOG(_severity)                                                             \
    BOOST_LOG_STREAM_WITH_PARAMS(                                                  \
        boost::log::trivial::logger::get(),                                        \
        (sugo::log::setThreadAttribute<std::string>(                               \
            "File", boost::filesystem::path(__FILE__).filename().string()))(       \
            sugo::log::setThreadAttribute<int>("Line", __LINE__))(                 \
            sugo::log::setThreadAttribute<std::string>("Function", __FUNCTION__))( \
            boost::log::keywords::severity = ::boost::log::trivial::_severity))

template <typename ValueT>
inline ValueT setThreadAttribute(const char* name, ValueT value)
{
    auto attr = boost::log::attribute_cast<boost::log::attributes::mutable_constant<ValueT>>(
        boost::log::core::get()->get_thread_attributes()[name]);
    attr.set(value);
    return attr.get();
}
}  // namespace log

class Logger final
{
public:
    /// @brief Severity type
    using Severity = boost::log::trivial::severity_level;
    /// @brief Default severity
    static constexpr Severity DefaultSeverity = Severity::debug;

    static void reinit(const std::string& instanceName = std::string());
    static void init(Severity severity = DefaultSeverity, const std::string& instanceName = std::string());

private:
    Logger() {}

    static Severity m_severity;
};

}  // namespace sugo

#endif /* LOGGER_HPP_ */
