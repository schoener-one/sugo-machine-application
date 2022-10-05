///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2019, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-11-17
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Globals.hpp"
#include "IStateMachine.hpp"
#include "ServiceComponent.hpp"

#include <mutex>

namespace sugo
{
/**
 * Class to receive messages and drives a finite state machine by events.
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 *
 * @todo Needs to be transformed to a base class of all ServiceComponents in order
 *       to handle messages in a generic manner!
 */
template <class StateT, class EventT>
class StatedServiceComponent : public ServiceComponent
{
public:
    /**
     * @brief Construct a new stated service component object.
     *
     * @param messageBroker        The message broker to be used for sending and receiving.
     * @param stateMachine         The state machine of this component.
     */
    explicit StatedServiceComponent(ICommandMessageBroker&         messageBroker,
                                    IStateMachine<StateT, EventT>& stateMachine)
        : ServiceComponent(messageBroker), m_stateMachine(stateMachine)
    {
        messageBroker.registerPostProcessHandler(
            std::bind(&StatedServiceComponent::processAllEvents, this));
    }

protected:
    /**
     * @brief Processes all events queued for the state machine.
     */
    void processAllEvents()
    {
        while (!m_stateMachine.getEventQueue().empty())
        {
            (void)m_stateMachine.processNextEvent();
        }
    }

    message::CommandResponse handleStateChangeCommand(const message::Command& command,
                                                      const EventT&           event)
    {
        LOG(debug) << "Received command '" << command.name() << "' in state "
                   << m_stateMachine.getCurrentState();
        // FIXME do a state pre check here if a push would succeeded in current state!
        if (m_stateMachine.push(event))
        {
            return createResponse(command);
        }
        else
        {
            return createErrorResponse(
                command,
                Json({{protocol::IdErrorReason, protocol::IdErrorRequestTooMuchRequests}}));
        };
    }

    message::CommandResponse handleCommandGetState(const message::Command& command)
    {
        return createResponse(command,
                              Json({{protocol::IdState, m_stateMachine.getCurrentState()}}));
    }

private:
    IStateMachine<StateT, EventT>& m_stateMachine;  ///< The component state machine.
};

}  // namespace sugo
