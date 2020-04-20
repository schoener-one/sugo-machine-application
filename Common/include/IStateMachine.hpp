///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ISTATEMACHINE_HPP_
#define ISTATEMACHINE_HPP_

#include <functional>

namespace moco
{
/**
 * Interface class for finite state machines.
 * @tparam StateT State type
 * @tparam EventT Event type
 */
template <class StateT = int, class EventT = int>
class IStateMachine
{
public:
    virtual ~IStateMachine() {}

    /**
     * Transition action function type.
     */
    using Action = std::function<void(const EventT &, const StateT &)>;

    /**
     * Returns the current state.
     * @return the current state.
     */
    virtual StateT getCurrentState() const = 0;

    /**
     * Processes a new event according to the current state and the transition
     * table.
     *
     * @param event              Event to be processed.
     * @param transitionNotFound Action to be called if a transition could not be found.
     */
    virtual void push(const EventT &event, Action transitionNotFound) = 0;

protected:
    IStateMachine() {}
};
}  // namespace moco

#endif  // ISTATEMACHINE_HPP_
