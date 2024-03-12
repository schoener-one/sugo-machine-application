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

#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>

#include "Common/Types.hpp"
#include "ServiceComponent/EventQueue.hpp"
#include "ServiceComponent/IStateMachine.hpp"

namespace sugo::service_component
{
/// @brief Generic class provides a finite state machine.
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

    bool processNextEvent() override
    {
        std::lock_guard<std::mutex> lock(m_mutexEventProcessing);
        EventT                      event{};

        if (!m_eventQueue.pull(event))
        {
            // Not an error but queue could be reset!
            return false;
        }

        if (!processEvent(event))
        {
            LOG(fatal) << "Transition for event " << event << " not found in state " << m_state;
            ASSERT_NOT_REACHABLE;  // FIXME Print a warning other than stop system by fatal error!
        }

        return true;
    }

    IQueue<EventT>& getEventQueue() override
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
    //FIXME Move event queue out of the state machine!
    EventQueue<EventT>    m_eventQueue;            ///< Event queue for pending events.
    mutable std::mutex    m_mutexEventProcessing;  ///< Mutex to protect the state
};

}  // namespace sugo::service_component
