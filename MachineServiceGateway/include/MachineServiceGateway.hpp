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

#include "ICommandMessageBroker.hpp"
#include "IConfiguration.hpp"
#include "IMachineServiceGateway.hpp"
#include "IOContext.hpp"
#include "Server.hpp"
#include "ServiceComponent.hpp"

namespace sugo
{
/**
 * Class to offer the service interface on network.
 */
class MachineServiceGateway : public IMachineServiceGateway,
                              public ServiceComponent,
                              public Server::IMessageHandler
{
public:
    static void addConfigurationOptions(IConfiguration& configuration);

    /**
     * Creates a new instance
     */
    explicit MachineServiceGateway(ICommandMessageBroker& messageBroker, IOContext& ioContext,
                                   const IConfiguration& configuration);

    bool start() override;
    void stop() override;
    bool isRunning() const override;

protected:
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    void processPost() override{};

private:
    jsonrpcpp::Response processCommand(jsonrpcpp::request_ptr request);

    Server m_jsonRpcServer;
};
}  // namespace sugo
