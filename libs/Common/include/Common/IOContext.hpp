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

#include <boost/asio/io_context.hpp>

#include "Common/ProcessContext.hpp"

namespace sugo::common
{
// FIXME Rename to IOProcessContext
/// @brief Class representing the boost asio io context.
class IOContext : public ProcessContext
{
public:
    /**
     * @brief Construct a new io context object.
     *
     * @param instanceId Instance identifier of this context.
     * @param policy     Thread policy.
     * @param priority   Thread priority.
     */
    explicit IOContext(const std::string& instanceId, Thread::Policy policy = Thread::DefaultPolicy,
                       int priority = Thread::DefaultPriority);

    /// @brief Default destructor.
    ~IOContext() override = default;

    /// @brief Copy constructor
    IOContext(const IOContext&) = delete;

    /// @brief Move constructor.
    IOContext(IOContext&& ioContext) = delete;

    /// @brief Copy operator.
    IOContext& operator=(const IOContext&) = delete;

    /// @brief Move operator.
    IOContext& operator=(IOContext&& ioContext) = delete;

    /**
     * @brief Get the asio io_context.
     *
     * @return The reference to the asio io_context.
     */
    boost::asio::io_context& getContext()
    {
        return m_ioContext;
    }

private:
    boost::asio::io_context m_ioContext{};
};

}  // namespace sugo::common
