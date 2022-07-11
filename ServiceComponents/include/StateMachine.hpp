///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

#include <algorithm>
#include <cassert>
#include <mutex>
#include <queue>
#include <vector>

#include "Globals.hpp"
#include "IStateMachine.hpp"

namespace sugo
{
/**
 * Basic finite state machine.
 */
template <class StateT = int, class EventT = int>
class StateMachine : public IStateMachine<StateT, EventT>
{
public:
    using typename IStateMachine<StateT, EventT>::Action;

    /**
     * Transition guard function type.
     */
    using Guard = std::function<bool(void)>;

    /**
     * Transition type.
     */
    struct Transition
    {
        Transition(StateT _state, StateT _next, EventT _event, Action _action = nullptr,
                   Guard _guard = nullptr)
            : state(_state), next(_next), event(_event), action(_action), guard(_guard)
        {
        }

        const StateT state;   ///< Current state for transition.
        const StateT next;    ///< The next state after transition.
        const EventT event;   ///< The id of the event to react to.
        const Action action;  ///< Action function to be process on state change.
        const Guard  guard;   ///< Guard function to be checked for transition.
    };

    /**
     * Transition table type.
     */
    using TransitionTable = std::vector<Transition>;

    /**
     * Helper method to create a new action function in the transition table.
     *
     * @param _func Action function to be called.
     * @param _obj  Instance to be called.
     * @return      Action object.
     */
    template <class ClassT, class BFuncT>
    static Action MakeAction(BFuncT&& _func, ClassT* _instance)
    {
        return std::bind(_func, _instance, std::placeholders::_1, std::placeholders::_2);
    }

    /**
     * Helper method to create a new guard function in the transition table.
     *
     * @param _func Action function to be called.
     * @param _obj  Instance to be called.
     * @return      Guard object.
     */
    template <class ClassT, class BFuncT>
    static Guard MakeGuard(BFuncT&& _func, ClassT* _instance)
    {
        return std::bind(_func, _instance);
    }

protected:
    /**
     * Constructor of the class.
     *
     * @param initState          Initial state of the machine.
     * @param transitions        Definition of the transition table.
     * @param transitionNotFound Definition of the handler method for not
     *                           existing transitions.
     */
    StateMachine(StateT initState, const TransitionTable transitions)
        : m_state{initState}, m_transitions{transitions}
    {
    }

public:
    virtual ~StateMachine() {}

    // IStateMachine {{
    StateT getCurrentState() const override { return m_state; }

    void push(const EventT& event, Action transitionNotFound = nullptr)
    {
        m_queueEvents.push(event);
        std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
        if (lock.owns_lock())  // Only if holding the lock, otherwise don't block!
        {
            bool success = true;
            while (success && (!m_queueEvents.empty()))
            {
                success = processEvent(m_queueEvents.front());
                m_queueEvents.pop();
                if (!success)
                {
                    LOG(error) << "Transition for event '" << event << "' not found in state '"
                               << m_state << "'";
                    if (transitionNotFound != nullptr)
                    {
                        transitionNotFound(event, m_state);
                    }
                    else
                    {
                        ASSERT_NOT_REACHABLE;
                    }
                }
            }
        }
    }
    // IStateMachine }}

private:
    /**
     * Processes a single event.
     *
     * @param event The event to be processed.
     * @return True if a transition could be found for the event.
     */
    bool processEvent(const EventT& event)
    {
        typename TransitionTable::const_iterator ti = m_transitions.end();
        ti           = std::find_if(m_transitions.begin(), m_transitions.end(),
                          [&](const Transition& arg) -> bool {
                              return m_state == arg.state && event == arg.event &&
                                     (arg.guard == nullptr || arg.guard());
                          });
        bool success = false;
        if (ti != m_transitions.end())
        {
            LOG(debug) << "Transition for event '" << event << "' from state '" << m_state
                       << "' to state '" << ti->next << "'";
            m_state = ti->next;
            if (ti->action != nullptr)
            {
                ti->action(event, m_state);
            }
            success = true;
        }
        return success;
    }

    StateT                m_state;        ///< Current state.
    std::mutex            m_mutex;        ///< Mutex to protect against race conditions.
    const TransitionTable m_transitions;  ///< Transition table.
    // FIXME use a fixed size queue here!
    std::queue<EventT> m_queueEvents;  ///< Pending events.
};

}  // namespace sugo

#endif  // STATEMACHINE_HPP_
