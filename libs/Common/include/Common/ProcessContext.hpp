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

#include <functional>
#include <mutex>

#include "Common/IProcessContext.hpp"
#include "Common/Thread.hpp"

namespace sugo::common
{
/// @brief Class representing a process context.
class ProcessContext : public IProcessContext
{
public:
    /**
     * @brief Construct a new work context object
     *
     * @param instanceId Identifier for this instance.
     * @param policy     Thread policy to be used.
     * @param priority   Priority to be apply to the working instances.
     */
    explicit ProcessContext(const std::string& instanceId,
                            Thread::Policy     policy   = Thread::DefaultPolicy,
                            Thread::Priority   priority = Thread::DefaultPriority)
        : m_thread(instanceId, policy, priority)
    {
    }

    /// @brief Default destructor.
    ~ProcessContext() override = default;

    /// @brief Copy constructor
    ProcessContext(const ProcessContext&) = delete;

    /**
     * @brief Move constructor.
     *
     * @param processContext Process context to move to this.
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    ProcessContext(ProcessContext&& processContext)
        : m_thread(std::move(processContext.m_thread)),
          m_process(std::move(processContext.m_process)),
          m_stopProcess(std::move(processContext.m_stopProcess))
    {
    }

    /// @brief Copy operator.
    /// @return This object.
    ProcessContext& operator=(const ProcessContext&) = delete;

    /**
     * @brief Move operator.
     *
     * @param processContext Process context to move to this.
     * @return This object.
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    ProcessContext& operator=(ProcessContext&& processContext)
    {
        *this = std::move(processContext);
        return *this;
    }

    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    bool start() override;
    void stop() override;
    void waitUntilFinished() override;

    /**
     * @brief Set the process runner function to be used on start from this context.
     * Must not be set during running!
     *
     * @param process Process to be used.
     * @return true  If the process has not yet started and could be set.
     * @return false If the process has already started.
     */
    bool setProcessRunner(Thread::Runnable process,
                          Thread::Runnable stopProcess = nullptr) override;

private:
    Thread           m_thread;                 ///< Main worker thread
    Thread::Runnable m_process     = nullptr;  ///< Process function to run
    Thread::Runnable m_stopProcess = nullptr;  ///< Function to stop a running process
    std::mutex       m_mutex;                  ///< Mutex
};

}  // namespace sugo::common
