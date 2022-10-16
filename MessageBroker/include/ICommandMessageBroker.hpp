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

#include "IGenericMessageBroker.hpp"
#include "Message.hpp"

namespace sugo::message
{
/// Interface definition of the CommandMessageBroker.
using ICommandMessageBroker =
    IGenericMessageBroker<message::Command, MessageId, message::CommandResponse, Address>;
}  // namespace sugo::message
