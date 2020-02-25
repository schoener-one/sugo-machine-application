/*
 * CoffeeAutomatService.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMATSERVICE_HPP_
#define COFFEEAUTOMATSERVICE_HPP_

#include "CommunicationService.hpp"
#include "ICommandMessageBroker.hpp"

namespace moco
{

class CoffeeAutomatService : public CommunicationService
{
public:
	/**
	 * Creates a new instance
	 */
	CoffeeAutomatService(const std::string & serviceAddress, ICommandMessageBroker& messageBroker);

protected:
	bool processMessage(const std::string& message) override;

private:
	ICommandMessageBroker& m_messageBroker;
};
}

#endif /* COFFEEAUTOMATSERVICE_HPP_ */
