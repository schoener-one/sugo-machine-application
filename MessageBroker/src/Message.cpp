///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "Message.hpp"

using namespace sugo::message;

std::ostream& sugo::message::operator<<(std::ostream& ostr, const message::CommandId& commandId)
{
    ostr << commandId.getReceiverAddress() << "/" << commandId.getId();
    return ostr;
}

std::ostream& sugo::message::operator<<(std::ostream& ostr, const message::NotificationId& notificationId)
{
    ostr << notificationId.getPublisherAddress() << "/" << notificationId.getId();
    return ostr;
}
