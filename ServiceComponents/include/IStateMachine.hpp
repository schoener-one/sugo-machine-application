///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EventQueue.hpp"

#include <functional>

namespace sugo
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
    virtual EventQueue<EventT> &getEventQueue() = 0;

    /**
     * @brief Checks if the event could be processed successfully.
     *
     * @param event  Event to be checked according to the current state.
     * @return true  If the could be processed successfully.
     * @return false If the could not be processed successfully.
     */
    virtual bool checkNextEvent(const EventT &event) const = 0;
};
}  // namespace sugo
