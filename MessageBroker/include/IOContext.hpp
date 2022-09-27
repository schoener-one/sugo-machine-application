///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   08.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProcessContext.hpp"

#include <boost/asio/io_context.hpp>

namespace sugo
{
/**
 * Class representing the io context.
 */
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
    IOContext(const std::string& instanceId, Thread::Policy policy = Thread::DefaultPolicy,
              int priority = Thread::DefaultPriority);

    /**
     * @brief Get the asio io_context.
     *
     * @return The reference to the asio io_context.
     */
    boost::asio::io_context& getContext()
    {
        return m_asioContext;
    }

private:
    boost::asio::io_context m_asioContext;
};

}  // namespace sugo
