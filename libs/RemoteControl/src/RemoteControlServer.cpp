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

#include <mongoose.h>
#include <boost/regex.hpp>
#include <cassert>
#include <mutex>

#include "Common/Logger.hpp"
#include "Common/Types.hpp"
#include "RemoteControl/Configuration.hpp"
#include "RemoteControl/RemoteControlServer.hpp"

using namespace sugo::remote_control;

RemoteControlServer::RemoteControlServer(const common::IConfiguration &configuration,
                                         IClientRequestHandler &       requestHandler)
    : RemoteControlServer(configuration.getOption(id::ConfigNetworkAddress).get<std::string>(),
                          configuration.getOption(id::ConfigNetworkPort).get<unsigned short>(),
                          configuration.getOption(id::ConfigDocRoot).get<std::string>(),
                          requestHandler)
{
}

RemoteControlServer::RemoteControlServer(std::string address, unsigned short port,
                                         std::string docRoot, IClientRequestHandler &requestHandler)
    : m_address(std::move(address)),
      m_port(port),
      m_docRoot(std::move(docRoot)),
      m_requestHandler(requestHandler),
      m_serverStatus(std::make_unique<mg_mgr>()),
      m_thread("RemoteControlServer")
{
    mg_log_set(MG_LL_ERROR);
    requestHandler.registerSendNotification(
        [&](const common::Json &notification) { sendNotification(notification); });
}

RemoteControlServer::~RemoteControlServer()  // NOLINT(modernize-use-equals-default) -
                                             // std::unique_ptr/std::default_delete sizeof unknown
{
}

bool RemoteControlServer::start()
{
    std::lock_guard<std::mutex> lock(m_mutexStartStop);
    if (isRunning())
    {
        LOG(warning) << "Instance already running";
        return false;
    }

    return m_thread.start([&] {
        mg_mgr_init(m_serverStatus.get());
        (void)listen();
    });
}

void RemoteControlServer::stop()
{
    std::lock_guard<std::mutex> lock(m_mutexStartStop);
    if (isRunning())
    {
        m_doListen = false;
        waitUntilFinished();
        mg_mgr_free(m_serverStatus.get());
    }
}

void RemoteControlServer::sendNotification(const common::Json &notification)
{
    std::lock_guard<std::mutex> lock(m_mutexStartStop);
    for (auto &[key, connection] : m_connections)
    {
        if (connection.isOpenWebsocket())
        {
            connection.sendMessage(notification);
        }
    }
}

void RemoteControlServer::handleEvent(mg_connection *mgConnection, int event, void *eventData)
{
    switch (event)
    {
        case MG_EV_HTTP_MSG:  // HTTP request/response        struct mg_http_message *
            getConnection(mgConnection->id)
                .handleHttpMessage(static_cast<mg_http_message *>(eventData), m_docRoot);
            break;
        case MG_EV_ERROR:  // Error                        char *error_message
            getConnection(mgConnection->id).handleError(static_cast<const char *>(eventData));
            break;
        case MG_EV_OPEN:  // Connection created           NULL
        {
            std::lock_guard<std::mutex> lock(m_mutexStartStop);
            m_connections[mgConnection->id] = Connection(mgConnection);
        }
            LOG(debug) << "New connection: " << mgConnection->id;
            break;
        case MG_EV_CLOSE:  // Connection closed            NULL
        {
            std::lock_guard<std::mutex> lock(m_mutexStartStop);
            m_connections.erase(mgConnection->id);
        }
            LOG(debug) << "Connection closed: " << mgConnection->id;
            break;
        case MG_EV_WS_OPEN:  // Websocket handshake done     struct mg_http_message *
            getConnection(mgConnection->id)
                .handleWebsocketOpen(static_cast<mg_http_message *>(eventData));
            break;
        case MG_EV_WS_MSG:  // Websocket msg, text or bin   struct mg_ws_message *
            getConnection(mgConnection->id)
                .handleWebsocketMessage(static_cast<mg_ws_message *>(eventData), m_requestHandler);
            break;
        case MG_EV_WS_CTL:  // Websocket control msg        struct mg_ws_message *
            getConnection(mgConnection->id)
                .handleWebsocketControl(static_cast<mg_ws_message *>(eventData));
            break;
        case MG_EV_RESOLVE:     // Host name is resolved        NULL
        case MG_EV_CONNECT:     // Connection established       NULL
        case MG_EV_ACCEPT:      // Connection accepted          NULL
        case MG_EV_READ:        // Data received from socket    struct mg_str *
        case MG_EV_WRITE:       // Data written to socket       long *bytes_written
        case MG_EV_HTTP_CHUNK:  // HTTP chunk (partial msg)     struct mg_http_message *
        case MG_EV_MQTT_CMD:    // MQTT low-level command       struct mg_mqtt_message *
        case MG_EV_MQTT_MSG:    // MQTT PUBLISH received        struct mg_mqtt_message *
        case MG_EV_MQTT_OPEN:   // MQTT CONNACK received        int *connack_status_code
        case MG_EV_SNTP_TIME:   // SNTP time received           uint64_t *milliseconds
            LOG(trace) << "Event " << event << " not handled";
            break;
        case MG_EV_POLL:  // mg_mgr_poll iteration        uint64_t *milliseconds
            LOG(trace) << "Event " << event << " not handled";
            break;
        default:
            LOG(warning) << "Event unknown";
            break;
    }
}

bool RemoteControlServer::listen()
{
    const std::string url = m_address + ":" + std::to_string(m_port);
    LOG(debug) << "Starting listening on '" + url + "'";
    const auto *connection = mg_http_listen(
        m_serverStatus.get(), url.c_str(),
        [](mg_connection *connection, int event, void *eventData, void *data) {
            assert(data != nullptr);
            auto *server = static_cast<RemoteControlServer *>(data);
            server->handleEvent(connection, event, eventData);
        },
        this);
    if (connection == nullptr)
    {
        LOG(error) << "Failed to start listening";
        return false;
    }

    for (m_doListen = true; m_doListen;)
    {
        static constexpr int pollTimeoutMs =
            1000;  // TODO request value from configuration interface!
        mg_mgr_poll(m_serverStatus.get(), pollTimeoutMs);
    }
    m_connections.clear();
    LOG(debug) << "Finish listening";
    return true;
}