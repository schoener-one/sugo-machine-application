///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
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

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "ServiceComponent/IQueue.hpp"

namespace sugo::service_component
{
/**
 * @brief common::Thread safe event queue.
 *
 * @tparam EventT Event type to be queued.
 */
template <class EventT>
class EventQueue : public IQueue<EventT>
{
public:
    /// @brief Default queue size.
    constexpr static std::size_t DefaultQueueSize = 10u;

    /**
     * @brief Construct a new event queue object.
     *
     * @param maxQueueSize Maximum size of the event queue
     */
    explicit EventQueue(const std::size_t maxQueueSize = DefaultQueueSize) : m_maxSize(maxQueueSize)
    {
    }

    /// @brief Copy constructor
    EventQueue(const EventQueue&) = delete;

    /// @brief Move constructor
    EventQueue(EventQueue&&) = delete;

    /// @brief Copy operator
    /// @return EventQueue
    EventQueue& operator=(const EventQueue&) = delete;

    /// @brief Move operator
    /// @return EventQueue
    EventQueue& operator=(EventQueue&&) = delete;

    ~EventQueue() override = default;

    bool push(const EventT& event) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_maxSize == (m_queue.size() + 1))
        {
            return false;
        }
        m_queue.push(event);
        m_condVar.notify_one();
        return true;
    }

    bool pull(EventT& event) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (empty())
        {
            m_condVar.wait(lock);
            if (empty())
            {
                return false;
            }
        }

        event = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool empty() const override
    {
        return m_queue.empty();
    }

    void reset() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::queue<EventT>           emptyQueue{};
        m_queue.swap(emptyQueue);
        m_condVar.notify_one();
    }

private:
    std::queue<EventT>      m_queue;        ///< Pending events.
    const std::size_t       m_maxSize = 0;  ///< Max queue size.
    std::mutex              m_mutex;        ///< The mutex.
    std::condition_variable m_condVar;      ///< Queue fill condition.
};
}  // namespace sugo::service_component
