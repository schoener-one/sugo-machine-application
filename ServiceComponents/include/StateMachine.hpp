///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2020-05-02
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EventQueue.hpp"
#include "Globals.hpp"
#include "IStateMachine.hpp"

#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>

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
    using typename IStateMachine<StateT, EventT>::Guard;

    /**
     * Transition type.
     */
    struct Transition
    {
        Transition(StateT paramState, StateT paramNext, EventT paramEvent,
                   Action paramAction = nullptr, Guard paramGuard = nullptr)
            : state(paramState),
              next(paramNext),
              event(paramEvent),
              action(std::move(paramAction)),
              guard(std::move(paramGuard))
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
     * @param func Action function to be called.
     * @param instance  Instance to be called.
     * @return      Action object.
     */
    template <class _ClassT, class _FuncT>
    static Action MakeAction(_FuncT&& func, _ClassT* instance)
    {
        return std::bind(func, instance, std::placeholders::_1, std::placeholders::_2);
    }

    /**
     * Helper method to create a new guard function in the transition table.
     *
     * @param func Action function to be called.
     * @param instance  Instance to be called.
     * @return      Guard object.
     */
    template <class _ClassT, class _FuncT>
    static Guard MakeGuard(_FuncT&& func, _ClassT* instance)
    {
        return std::bind(func, instance);
    }

    /**
     * Constructor of the class.
     *
     * @param initState          Initial state of the machine.
     * @param transitions        Definition of the transition table.
     * @param transitionNotFound Definition of the handler method for not
     *                           existing transitions.
     */
    StateMachine(StateT initState, const TransitionTable& transitions)
        : m_state{initState}, m_transitions{transitions}
    {
    }
    ~StateMachine() override = default;

    StateT getCurrentState() const override
    {
        return m_state;
    }

    bool push(const EventT& event) override
    {
        return m_eventQueue.push(event);
    }

    bool checkNextEvent(const EventT& event) const override
    {
        std::lock_guard<std::mutex> lock(m_mutexEventProcessing);
        const auto                  ti = findTransition(event);
        return (ti != m_transitions.end());
    }

    bool processNextEvent() override
    {
        std::lock_guard<std::mutex> lock(m_mutexEventProcessing);

        if (m_eventQueue.empty())
        {
            return false;
        }

        EventT event = m_eventQueue.pull();

        if (!processEvent(event))
        {
            LOG(fatal) << "Transition for event " << event << " not found in state " << m_state;
            ASSERT_NOT_REACHABLE;
        }

        return true;
    }

    EventQueue<EventT>& getEventQueue() override
    {
        return m_eventQueue;
    }

private:
    /**
     * Processes a single event.
     *
     * @param event The event to be processed.
     * @return True if a transition could be found for the event.
     */
    bool processEvent(const EventT& event)
    {
        auto ti = findTransition(event);

        if (ti == m_transitions.end())
        {
            return false;
        }

        LOG(debug) << "Transition for event " << event << " from state " << m_state << " to state "
                   << ti->next;
        m_state = ti->next;

        if (ti->action != nullptr)
        {
            ti->action(event, m_state);
        }

        return true;
    }

    typename TransitionTable::const_iterator findTransition(const EventT& event) const
    {
        return std::find_if(m_transitions.begin(), m_transitions.end(),
                            [&](const Transition& arg) -> bool {
                                return m_state == arg.state && event == arg.event &&
                                       (arg.guard == nullptr || arg.guard());
                            });
    }

    StateT                m_state;                 ///< Current state.
    const TransitionTable m_transitions;           ///< Transition table.
    EventQueue<EventT>    m_eventQueue;            ///< Event queue for pending events.
    mutable std::mutex    m_mutexEventProcessing;  ///< Mutex to protect the state
};

}  // namespace sugo
