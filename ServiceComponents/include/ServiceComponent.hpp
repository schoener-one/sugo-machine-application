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
#include "ICommandMessageBroker.hpp"
#include "IRunnable.hpp"
#include "MessageHelper.hpp"
#include "MessageId.hpp"

#include <string>

namespace sugo
{
/**
 * Class to receive command messages and to translate them to events.
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 *
 * @todo Needs to be transformed to a base class of all ServiceComponents in order
 *       to handle messages in a generic manner!
 */
class ServiceComponent : public IRunnable
{
public:
    /// @brief Type definition of message id.
    using MessageId = std::string;
    /// @brief Type definition of command id.
    using CommandId = GenericCommandId<MessageId, ICommandMessageBroker::ReceiverId>;
    /// @brief Type definition of notification id.
    using NotificationId = GenericNotificationId<MessageId, ICommandMessageBroker::ReceiverIdList>;

    /**
     * @brief Construct a new service component object
     *
     * @param messageBroker  The message broker to be used for sending and receiving.
     */
    explicit ServiceComponent(ICommandMessageBroker& messageBroker) : m_messageBroker(messageBroker)
    {
    }
    virtual ~ServiceComponent() = default;

    bool start() override
    {
        return m_messageBroker.start();
    }

    void stop() override
    {
        m_messageBroker.stop();
    }

    bool isRunning() const override
    {
        return m_messageBroker.isRunning();
    }

    /**
     * @brief Returns the command message broker.
     *
     * @return The command message broker.
     */
    ICommandMessageBroker& getCommandMessageBroker()
    {
        return m_messageBroker;
    }

protected:
    static message::CommandResponse createUnsupportedCommandResponse(
        const message::Command& command);

    static message::CommandResponse createResponse(
        const message::Command& command, const Json& response = Json(),
        const message::CommandResponse_Result result = message::CommandResponse_Result_SUCCESS)
    {
        return message::createResponse(command, response, result);
    }

    static message::CommandResponse createErrorResponse(
        const message::Command& command,
        const Json& errorMessage = Json({{protocol::IdErrorReason, protocol::IdErrorUnspecified}}),
        message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
    {
        return message::createErrorResponse(command, errorMessage, errorCode);
    }

    void registerMessageHandler(const CommandId& command, ICommandMessageBroker::Handler&& handler)
    {
        m_messageBroker.registerHandler(command.getId(), handler);
    }

    void registerMessageHandler(const NotificationId&            notification,
                                ICommandMessageBroker::Handler&& handler)
    {
        m_messageBroker.registerHandler(notification.getId(), handler);
    }

    message::CommandResponse send(const CommandId& commandId)
    {
        return send(commandId, Json());
    }

    message::CommandResponse send(const CommandId& commandId, const Json& parameters);
    message::CommandResponse forward(const CommandId& commandId, const message::Command& command);

    bool notify(const NotificationId& notificationId)
    {
        return notify(notificationId, "");
    }

    bool notify(const NotificationId& notificationId, const Json& parameters);

private:
    ICommandMessageBroker& m_messageBroker;
};

std::ostream& operator<<(std::ostream& ostr, const ServiceComponent::CommandId& commandId);
std::ostream& operator<<(std::ostream&                           ostr,
                         const ServiceComponent::NotificationId& notificationId);
}  // namespace sugo
