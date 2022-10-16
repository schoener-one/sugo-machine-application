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
#include "MessageProtocol.hpp"
#include "ServiceComponent.hpp"

#include <cassert>
#include <mutex>

namespace sugo
{
/**
 * Base class for service components which are driven by finite state machine (FSM).
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 */
template <class StateT, class EventT>
class StatedServiceComponent : public ServiceComponent
{
public:
    /**
     * @brief Construct a new stated service component object.
     *
     * @param messageBroker   The message broker to be used for sending and receiving.
     * @param subscriptionIds List of notification ids to subscribe to.
     * @param stateMachine    The state machine of this component.
     */
    explicit StatedServiceComponent(message::ICommandMessageBroker& messageBroker,
                                    const NotificationIdList&       subscriptionIds,
                                    IStateMachine<StateT, EventT>&  stateMachine)
        : ServiceComponent(messageBroker, subscriptionIds), m_stateMachine(stateMachine)
    {
    }

    bool start() override
    {
        getCommandMessageBroker().registerPostProcessHandler(
            std::bind(&StatedServiceComponent::processAllEvents, this));
        return ServiceComponent::start();
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

    message::CommandResponse handleStateChangeMessage(const message::Command& command,
                                                      const EventT&           event)
    {
        if (!m_stateMachine.checkNextEvent(event))
        {
            LOG(warning) << "Ignoring received message '" << command.name() << "' in invalid state "
                         << m_stateMachine.getCurrentState();
            return message::createErrorCommandResponse(
                command,
                Json({{message::protocol::IdErrorReason, message::protocol::IdErrorInvalidState}}));
        }

        LOG(debug) << "Received message '" << command.name() << "' in state "
                   << m_stateMachine.getCurrentState();

        if (m_stateMachine.push(event))
        {
            return message::createCommandResponse(command);
        }
        else
        {
            return message::createErrorCommandResponse(
                command, Json({{message::protocol::IdErrorReason,
                                message::protocol::IdErrorRequestTooMuchRequests}}));
        };
    }

    message::CommandResponse handleCommandGetState(const message::Command& command)
    {
        return message::createCommandResponse(
            command, Json({{message::protocol::IdState, m_stateMachine.getCurrentState()}}));
    }

private:
    IStateMachine<StateT, EventT>& m_stateMachine;  ///< The component state machine.
};

}  // namespace sugo
