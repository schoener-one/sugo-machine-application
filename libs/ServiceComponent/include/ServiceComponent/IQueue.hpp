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

namespace sugo::service_component
{
/**
 * @brief Interface class for Queues.
 *
 * @tparam EventT Element type which could be queued
 */
template <class EventT>
class IQueue
{
public:
    /// @brief Default destructor.
    virtual ~IQueue() = default;

    /**
     * @brief Pushes an element into the queue.
     *
     * @param element The element to push
     * @return true if the element could be pushed.
     * @return false if the element could not be pushed.
     */
    virtual bool push(const EventT& element) = 0;

    /**
     * @brief
     *
     * @return EventT Front element to be pulled out.
     */
    //
    /**
     * @brief Pulls an element out of the queue. This call blocks the
     * caller as long as no element is in the queue!
     *
     * @param event Contains the pulled event.
     * @return true If an event was successfully pulled from queue.
     * @return false If no event was pulled from queue, but the caller was waken up
     * by an notification or timeout event.
     */
    virtual bool pull(EventT& event) = 0;

    /**
     * @brief Indicates if the queue is empty or not.
     *
     * @return true  If the queue is empty
     * @return false If the queue is not empty
     */
    virtual bool empty() const = 0;

    /**
     * @brief Resets the queue and unblocks all on pull waiting threads.
     *
     */
    virtual void reset() = 0;
};
}  // namespace sugo::service_component