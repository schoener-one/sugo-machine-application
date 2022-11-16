///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   13.10.2019
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include <jsonrpcpp/jsonrpcpp.hpp>

#include "IConfiguration.hpp"
#include "IMachineServiceGateway.hpp"
#include "IMessageHandler.hpp"
#include "IOContext.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "ServiceComponent.hpp"

namespace sugo
{
/**
 * Class to offer the service interface on network.
 */
class MachineServiceGateway : public IMachineServiceGateway,
                              public ServiceComponent,
                              public message::IMessageHandler
{
public:
    static void addConfigurationOptions(IConfiguration& configuration);

    /**
     * Creates a new instance
     */
    explicit MachineServiceGateway(message::ICommandMessageBroker& messageBroker,
                                   message::IOContext&             ioContext,
                                   const IConfiguration&           configuration);

    bool start() override;
    void stop() override;
    bool isRunning() const override;

protected:
    bool processReceived(message::StreamBuffer& in, message::StreamBuffer& out) override;
    void processPost() override{};

private:
    jsonrpcpp::Response processCommand(const jsonrpcpp::request_ptr& request);

    message::Server m_jsonRpcServer;
};
}  // namespace sugo
