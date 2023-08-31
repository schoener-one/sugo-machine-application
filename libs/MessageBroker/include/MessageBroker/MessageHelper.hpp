///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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
// TODO move to ServiceComponent

#pragma once

#include "Common/Types.hpp"
#include "MessageBroker/Message.hpp"

namespace sugo::message_broker
{
/**
 * @brief Creates a response message.
 *
 * @param message     Message this response is based on.
 * @param result      Message execution result.
 * @param payload     Message payload.
 * @param errorReason Message error reason. Will only be used if the result is set to an error.
 * @return The response message.
 */
inline static ResponseMessage createResponseMessage(
    const Message& message, const ResponseMessage::Result result,
    const common::Json& payload = common::Json{}, const common::Json& errorReason = common::Json{})
{
    ResponseMessage responseMessage{};
    responseMessage.setResult(result);
    responseMessage.referTo(message);

    if (result != ResponseMessage::Result::Success)
    {
        responseMessage.setError(errorReason);
    }

    if (!payload.empty())
    {
        responseMessage.setPayload(payload);
    }

    return responseMessage;
}

/**
 * @brief Creates a response message with successful result.
 *
 * @param message     Message this response is based on.
 * @param payload     Message payload.
 * @return The response message.
 */
inline static ResponseMessage createResponseMessage(const Message&      message,
                                                    const common::Json& payload = common::Json{})
{
    return createResponseMessage(message, ResponseMessage::Result::Success, payload);
}

/**
 * @brief Creates a error response message.
 *
 * @param message     Message this response is based on.
 * @param result      Message execution result.
 * @return The response message.
 */
inline static ResponseMessage createErrorResponseMessage(const Message&                message,
                                                         const ResponseMessage::Result result)
{
    return createResponseMessage(
        message, result,
        common::Json{{message_broker::id::ErrorReason, ResponseMessage::getResultName(result)}});
}

}  // namespace sugo::message_broker
