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

#pragma once

#include <functional>

#include "Common/Types.hpp"

namespace sugo::remote_control
{
/// @brief Class which handles websocket requests
class IClientRequestHandler
{
public:
    /// @brief Uniq identifier for a request client.
    using ClientId = unsigned long;

    /// @brief Callback to send notifications to all clients.
    using SendNotificationCallback = std::function<void(const common::Json&)>;
    /**
     * @brief Called if a new request has been received.
     *
     * @param clientId Unique identifier for a request client.
     * @param request JSON structure which contains the client request.
     * @param response JSON object which receives the response.
     * @return true If the request could be handled and the response has been set.
     * @return false If the request was unknown.
     */
    virtual bool receiveRequest(ClientId clientId, const common::Json& request,
                                common::Json& response) = 0;

    /**
     * @brief Method to register a callback which could be used to send client notifications.
     *
     * @param callback Callback for sending notifications.
     */
    virtual void registerSendNotification(SendNotificationCallback callback) = 0;
};
}  // namespace sugo::remote_control