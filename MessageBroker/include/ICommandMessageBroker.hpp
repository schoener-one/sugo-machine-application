///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   20.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ICOMMANDMESSAGEBROKER_HPP_
#define ICOMMANDMESSAGEBROKER_HPP_

#include <Command.pb.h>
#include <string>

#include "IMessageBroker.hpp"

namespace sugo
{
/// Interface definition of the CommandMessageBroker.
using ICommandMessageBroker =
    IMessageBroker<message::Command, message::CommandResponse, std::string>;
}  // namespace sugo

#endif  // ICOMMANDMESSAGEBROKER_HPP_
