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

#include <cassert>
#include <mutex>

#include "Common/IProcessContext.hpp"
#include "Common/Logger.hpp"
#include "Common/Types.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/MessageHelper.hpp"
#include "ServiceComponent/IStateMachine.hpp"
#include "ServiceComponent/Protocol.hpp"
#include "ServiceComponent/ServiceComponent.hpp"

namespace sugo::service_component
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
     * @param processContext  Process context in which the event loop is running.
     */
    explicit StatedServiceComponent(message_broker::IMessageBroker& messageBroker,
                                    const NotificationIdList&       subscriptionIds,
                                    IStateMachine<StateT, EventT>&  stateMachine,
                                    common::IProcessContext&        processContext)
        : ServiceComponent(messageBroker, subscriptionIds),
          m_stateMachine(stateMachine),
          m_processContext(processContext)
    {
    }

    bool start() override
    {
        m_processEvents = true;
        m_processContext.setProcessRunner(
            [this] {
                while (this->m_processEvents)
                {
                    m_stateMachine.processNextEvent();
                }
            },
            [this] {
                this->m_processEvents = false;
                m_stateMachine.getEventQueue().reset();  // unblock
            });

        if (!m_processContext.start())
        {
            LOG(error) << "Failed to start process context";
            return false;
        }

        return ServiceComponent::start();
    }

    void stop() override
    {
        m_processContext.stop();
        ServiceComponent::stop();
    }

    bool isRunning() const override
    {
        return ServiceComponent::isRunning() && m_processContext.isRunning();
    }

protected:
    /**
     * @brief Method to handle request messages.
     *
     * @param message Received message to handle
     * @param event Event associated to the received message.
     * @return Request response message.
     */
    message_broker::ResponseMessage handleCommandRequestMessage(
        const message_broker::Message& message, const EventT& event)
    {
        if (m_stateMachine.push(event))
        {
            return message_broker::createResponseMessage(message);
        }
        else
        {
            return message_broker::createErrorResponseMessage(
                message, message_broker::ResponseMessage::Result::UnsupportedRequest);
        };
    }

    /**
     * @brief Method to handle notification messages.
     *
     * @param event Event associated to the received message.
     */
    void handleNotificationMessage(const EventT& event)
    {
        (void)m_stateMachine.push(event);
    }

    /**
     * @brief Helper method to handle state request messages.
     *
     * @param message Received message to handle
     * @return Request response message.
     */
    message_broker::ResponseMessage handleStateRequest(const message_broker::Message& message)
    {
        const common::Json parameter(
            {{id::State, static_cast<uint32_t>(m_stateMachine.getCurrentState())}});
        return message_broker::createResponseMessage(message, parameter);
    }

private:
    IStateMachine<StateT, EventT>& m_stateMachine;    ///< The component state machine.
    common::IProcessContext&       m_processContext;  ///< The process context.
    bool m_processEvents = false;                     ///< Indicates if events should be processed.
};

}  // namespace sugo::service_component
