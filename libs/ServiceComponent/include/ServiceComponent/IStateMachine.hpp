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

#include <functional>

#include "ServiceComponent/IQueue.hpp"

namespace sugo::service_component
{
/**
 * Interface class for finite state machines.
 * @tparam StateT State type
 * @tparam EventT Event type
 * @todo Remove IQueue regarding interface (i.e. push, wakeUpAll)
 */
template <class StateT = int, class EventT = int>
class IStateMachine
{
public:
    virtual ~IStateMachine() = default;

    /**
     * Transition action function type.
     */
    using Action = std::function<void(const EventT &, const StateT &)>;

    /**
     * Transition guard function type.
     */
    using Guard = std::function<bool(void)>;

    /**
     * Returns the current state.
     * @return the current state.
     */
    virtual StateT getCurrentState() const = 0;

    /**
     * @brief Processes a new event according to the current state and the transition table.
     * This call could be blocked by another calling thread!
     *
     * @param event  Event to be processed.
     * @return true  If the event could be pushed.
     * @return false If the event could not be pushed (i.e. queue full).
     */
    virtual bool push(const EventT &event) = 0;

    /**
     * @brief Process the next event pushed to the queue.
     * This call could be blocked by another calling thread or if the queue is empty!
     *
     * @return true  If the next event could be processed.
     * @return false If the next event could not be processed because of a transition not found
     * error.
     */
    virtual bool processNextEvent() = 0;

    /**
     * @brief Returns the event queue.
     *
     * @return EventQueue Event queue instance.
     */
    virtual IQueue<EventT> &getEventQueue() = 0;
};
}  // namespace sugo::service_component
