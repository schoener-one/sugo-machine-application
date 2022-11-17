///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Globals.hpp"
#include "IServiceComponent.hpp"
#include "Message.hpp"
#include "MessageHelper.hpp"

namespace sugo
{
/**
 * Class to receive command messages and to translate them to events.
 *
 */
class ServiceComponent : public IServiceComponent
{
public:
    /**
     * @brief Construct a new service component object
     *
     * @param messageBroker   The message broker to be used for sending and receiving.
     * @param subscriptionIds List of notification ids to subscribe to.
     */
    explicit ServiceComponent(message::ICommandMessageBroker& messageBroker,
                              const NotificationIdList&       subscriptionIds);
    ~ServiceComponent() override = default;

    bool start() override;

    void stop() override
    {
        m_messageBroker.stop();
    }

    bool isRunning() const override
    {
        return m_messageBroker.isRunning();
    }

    message::ICommandMessageBroker& getCommandMessageBroker() override
    {
        return m_messageBroker;
    }

protected:
    void registerMessageHandler(const message::CommandId&                        command,
                                message::ICommandMessageBroker::MessageHandler&& handler)
    {
        m_messageBroker.registerMessageHandler(command.getId(), handler);
    }

    void registerMessageHandler(const message::NotificationId&                   notification,
                                message::ICommandMessageBroker::MessageHandler&& handler)
    {
        m_messageBroker.registerMessageHandler(notification.getId(), handler);
    }

    message::CommandResponse send(const message::CommandId& commandId,
                                  const Json&               parameters = Json());
    message::CommandResponse forward(const message::CommandId& commandId,
                                     const message::Command&   command);
    bool notify(const message::NotificationId& notificationId, const Json& parameters = Json());

private:
    message::ICommandMessageBroker& m_messageBroker;
    const NotificationIdList&       m_subscriptionIds;
};
}  // namespace sugo
