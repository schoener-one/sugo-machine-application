///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   13.10.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef MACHINESERVICE_HPP_
#define MACHINESERVICE_HPP_

#include <string>

#include <jsonrpcpp/jsonrpcpp.hpp>

#include "ICommandMessageBroker.hpp"
#include "IConfiguration.hpp"
#include "IMachineService.hpp"
#include "Server.hpp"
#include "ServiceComponent.hpp"

namespace sugo
{
/**
 * Class to offer the service interface on network.
 */
class MachineService : public ServiceComponent, public Server::IMessageHandler
{
public:
    static void setConfiguration(IConfiguration& configuration);

    /**
     * Creates a new instance
     */
    explicit MachineService(ICommandMessageBroker& messageBroker,
                            const IConfiguration& configuration, IOContext& ioContext);

    // IRunnable {{
    bool start() override;

    void stop() override;

    bool isRunning() const override;
    // IRunnable }}

protected:
    // Server::IMessageHandler {{
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    // Server::IMessageHandler }}

private:
    jsonrpcpp::Response processCommand(jsonrpcpp::request_ptr request);

    Server m_jsonRpcServer;
};
}  // namespace sugo

#endif /* MACHINESERVICE_HPP_ */
