/*
 * CoffeeAutomatService.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMATSERVICE_HPP_
#define COFFEEAUTOMATSERVICE_HPP_

#include <string>

#include <jsonrpcpp/jsonrpcpp.hpp>

#include "ICoffeeAutomatService.hpp"
#include "ICommandMessageBroker.hpp"
#include "IConfiguration.hpp"
#include "Responder.hpp"
#include "ServiceComponent.hpp"

namespace moco
{
/**
 * Class to offer the service interface on network.
 */
class CoffeeAutomatService : public ServiceComponent, public Responder::IMessageHandler
{
public:
    static void setConfiguration(IConfiguration& configuration);

    /**
     * Creates a new instance
     */
    explicit CoffeeAutomatService(ICommandMessageBroker& messageBroker,
                                  const IConfiguration& configuration, IOContext& ioContext);

    // IRunnable {{
    bool start() override;

    void stop() override;

    bool isRunning() const override;
    // IRunnable }}

protected:
    // Responder {{
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    // Responder }}

private:
    jsonrpcpp::Response processCommand(jsonrpcpp::request_ptr request);

    Responder m_jsonRpcResponder;
};
}  // namespace moco

#endif /* COFFEEAUTOMATSERVICE_HPP_ */
