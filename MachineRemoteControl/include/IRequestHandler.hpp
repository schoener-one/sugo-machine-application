///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-09
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Globals.hpp"

#include <functional>

namespace sugo::remote_control
{
/**
 * @brief Class which handles websocket requests
 */
class IRequestHandler
{
public:
    /// @brief Uniq identifier for a request client.
    using ClientId = unsigned long;
    /// @brief Callback to send notifications to all clients.
    using SendNotificationCallback = std::function<void(const Json&)>;
    /**
     * @brief Called if a new request has been received.
     *
     * @param clientId Unique identifier for a request client.
     * @param request JSON structure which contains the client request.
     * @param response JSON object which receives the response.
     * @return true If the request could be handled and the response has been set.
     * @return false If the request was unknown.
     */
    virtual bool receiveRequest(ClientId clientId, const Json& request, Json& response) = 0;

    /**
     * @brief Method to register a callback which could be used to send client notifications.
     *
     * @param callback Callback for sending notifications.
     */
    virtual void registerSendNotification(SendNotificationCallback callback) = 0;
};
}  // namespace sugo::remote_control