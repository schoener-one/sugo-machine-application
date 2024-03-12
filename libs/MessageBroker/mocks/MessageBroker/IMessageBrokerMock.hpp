///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener , (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * , (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <gmock/gmock.h>

#include "MessageBroker/IMessageBroker.hpp"

namespace sugo::message_broker
{
/// @brief IMessageBroker mock.
class IMessageBrokerMock : public IMessageBroker
{
public:
    MOCK_METHOD(bool, start, ());
    MOCK_METHOD(void, stop, ());
    MOCK_METHOD(bool, isRunning, (), (const));
    MOCK_METHOD(bool, notify, (Message&, const Topic&));
    MOCK_METHOD(bool, subscribe, (const Address&, const Topic&));
    MOCK_METHOD(bool, unsubscribe, (const Address&, const Topic&));
    MOCK_METHOD(bool, send, (Message&, const Address&, ResponseMessage&));
    MOCK_METHOD(void, registerRequestMessageHandler, (RequestMessageHandler&));
    MOCK_METHOD(void, registerRequestMessageHandler, (RequestMessageHandler &&));
    MOCK_METHOD(void, registerNotificationMessageHandler, (NotificationMessageHandler&));
    MOCK_METHOD(void, registerNotificationMessageHandler, (NotificationMessageHandler &&));
    MOCK_METHOD(void, unregisterRequestMessageHandler, ());
    MOCK_METHOD(void, unregisterNotificationMessageHandler, ());
};

}  // namespace sugo::message_broker
