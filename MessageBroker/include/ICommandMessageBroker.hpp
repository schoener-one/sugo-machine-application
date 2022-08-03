///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   20.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Command.pb.h>
#include <string>

#include "IMessageBroker.hpp"

namespace sugo
{
/// Interface definition of the CommandMessageBroker.
using ICommandMessageBroker =
    IMessageBroker<message::Command, message::CommandResponse, std::string>;
}  // namespace sugo
