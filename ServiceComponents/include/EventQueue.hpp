///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-09-25
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <mutex>
#include <queue>

namespace sugo
{
/**
 * @brief Thread safe event queue.
 *
 * @tparam EventT Event type to be queued
 */
template <class EventT>
class EventQueue
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

    /**
     * @brief Pushes an element into the queue.
     *
     * @param element The element to push
     * @return true if the element could be pushed.
     * @return false if the element could not be pushed.
     */
    bool push(const EventT& event)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_maxSize == (m_queue.size() + 1))
        {
            return false;
        }
        m_queue.push(event);
        return true;
    }

    /**
     * @brief Pulls an element out of the queue.
     * The caller has to check if the queue is not empty!
     *
     * @return EventT Front element to be pulled out.
     */
    EventT pull()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        assert(!m_queue.empty());
        EventT event = std::move(m_queue.front());
        m_queue.pop();
        return event;
    }

    /**
     * @brief Indicates if the queue is empty or not.
     *
     * @return true  If the queue is empty
     * @return false If the queue is not empty
     */
    bool empty() const
    {
        return m_queue.empty();
    }

private:
    std::queue<EventT> m_queue;        ///< Pending events.
    const std::size_t  m_maxSize = 0;  ///< Max queue size.
    std::mutex         m_mutex;        ///< The mutex.
};

}  // namespace sugo
