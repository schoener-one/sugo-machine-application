/*
 * Message.hpp
 *
 *  Created on: 02.11.2019
 *      Author: denis
 */

#ifndef ICOMMANDMESSAGEBROKER_HPP_
#define ICOMMANDMESSAGEBROKER_HPP_

#include <Command.pb.h>
#include <string>

#include "IMessageBroker.hpp"
#include "MessageBroker.hpp"

namespace moco
{
	using ICommandMessageBroker = moco::IMessageBroker<message::Command,
			message::CommandResponse, std::string>;
	using CommandMessageBroker = moco::MessageBroker<message::Command,
			message::CommandResponse, std::string>;
} // namespace

#endif /* ICOMMANDMESSAGEBROKER_HPP_ */
