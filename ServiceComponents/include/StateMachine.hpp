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

    /**
     * Constructor of the class.
     *
     * @param initState          Initial state of the machine.
     * @param transitions        Definition of the transition table.
     * @param transitionNotFound Definition of the handler method for not
     *                           existing transitions.
     */
    StateMachine(StateT initState, const TransitionTable& transitions)
        : m_state{initState}, m_transitions{transitions}, m_eventQueue{}
    {
    }

    virtual ~StateMachine() override
    {
    }

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
        const auto ti = findTransition(event);
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
        return std::find_if(
            m_transitions.begin(), m_transitions.end(), [&](const Transition& arg) -> bool {
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
