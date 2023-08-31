///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/filesystem.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <string>

namespace sugo::common
{
namespace log
{
/// @brief Makro to define the logging stream object.
#define LOG(_severity)                                                                     \
    BOOST_LOG_STREAM_WITH_PARAMS(                                                          \
        boost::log::trivial::logger::get(),                                                \
        (sugo::common::log::setThreadAttribute<std::string>(                               \
            "File", boost::filesystem::path(__FILE__).filename().string()))(               \
            sugo::common::log::setThreadAttribute<int>("Line", __LINE__))(                 \
            sugo::common::log::setThreadAttribute<std::string>("Function", __FUNCTION__))( \
            boost::log::keywords::severity = ::boost::log::trivial::_severity))

/**
 * @brief Helper function to set the thread attribute.
 *
 * @tparam ValueT Type of the attribute to set.
 * @param name Name of the attribute to set.
 * @param value Value of the attribute.
 * @return Returns the new set value.
 */
template <typename ValueT>
inline ValueT setThreadAttribute(const char* name, ValueT value)
{
    auto attr = boost::log::attribute_cast<boost::log::attributes::mutable_constant<ValueT>>(
        boost::log::core::get()->get_thread_attributes()[name]);
    attr.set(value);
    return attr.get();
}
}  // namespace log

/**
 * @brief Class representing a logger interface.
 * This is a static only accessible class to provide an initialization of the
 * logger instance.
 */
class Logger final
{
public:
    /// @brief Severity type
    using Severity = boost::log::trivial::severity_level;
    /// @brief Default severity
    static constexpr Severity DefaultSeverity = Severity::debug;

    /**
     * @brief Reinitializes the logger instance according to the current thread.
     * @note If the logging should be used within a new thread context,
     * this has to be called first!
     *
     * @param instanceName Instance name of the logger.
     */
    static void reinit(const std::string& instanceName = std::string());

    /**
     * @brief Initializes the logger instance according to the current thread.
     *
     * @param severity Severity value for which the log messages have to be shown.
     * @param instanceName Instance name of the logger.
     */
    static void init(Severity           severity     = DefaultSeverity,
                     const std::string& instanceName = std::string());

private:
    /// @brief Default constructor
    Logger() = default;

    static Severity m_severity;  ///< Current global set severity.
};
}  // namespace sugo::common
