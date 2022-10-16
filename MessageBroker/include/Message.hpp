///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   16.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GenericMessageId.hpp"

#include <Command.pb.h>
#include <ostream>
#include <string>

namespace sugo::message
{
/// @brief Type definition of an address.
using Address = std::string;
/// @brief Type definition of message id.
using MessageId = std::string;
/// @brief Type definition of command id.
using CommandId = GenericCommandId<MessageId, Address>;
/// @brief Type definition of notification id.
using NotificationId = GenericNotificationId<MessageId, Address>;

/**
 * @brief Helper to print command ids.
 */
std::ostream& operator<<(std::ostream& ostr, const message::CommandId& commandId);
/**
 * @brief Helper to print notification ids.
 */
std::ostream& operator<<(std::ostream& ostr, const message::NotificationId& notificationId);
}  // namespace sugo::message