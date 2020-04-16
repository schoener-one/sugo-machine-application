/*
 * CoffeeAutomatService.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMATSERVICE_HPP_
#define COFFEEAUTOMATSERVICE_HPP_

#include <string>

#include "CommunicationService.hpp"
#include "ICommandMessageBroker.hpp"
#include "IConfiguration.hpp"

namespace moco
{
/**
 * Class to offer the service interface on network.
 */
class CoffeeAutomatService : public CommunicationService
{
public:
    static void setConfiguration(IConfiguration& configuration);

    /**
     * Creates a new instance
     */
    explicit CoffeeAutomatService(ICommandMessageBroker& messageBroker,
                                  const IConfiguration&  configuration);

    /**
     * Starts the service.
     * @return true if service could be started.
     */
    bool start();

protected:
    bool processMessage(const std::string& message) override;

private:
    ICommandMessageBroker& m_messageBroker;
    const std::string      m_serviceAddress;
};
}  // namespace moco

#endif /* COFFEEAUTOMATSERVICE_HPP_ */
