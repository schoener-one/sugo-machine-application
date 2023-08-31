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

#include <cassert>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Common/IConfiguration.hpp"
#include "Common/IRunnable.hpp"
#include "Common/Thread.hpp"
#include "Common/Types.hpp"
#include "IClientRequestHandler.hpp"
#include "RemoteControl/Connection.hpp"

struct mg_mgr;

namespace sugo::remote_control
{
/// @brief Class to handle HTTP and Websocket requests.
class RemoteControlServer : public common::IRunnable
{
public:
    /// @brief Maximum connection allowed to connect.
    static const unsigned MaxConnections = 3u;

    /**
     * @brief Construct a new remove control server.
     *
     * @param configuration  common::Configuration for this server.
     * @param requestHandler Request handler.
     */
    RemoteControlServer(const common::IConfiguration &configuration,
                        IClientRequestHandler &       requestHandler);
    /**
     * @brief Construct a new remove control server.
     *
     * @param address        Network interface address of the server for listening.
     * @param port           Port on which the service is provided.
     * @param docRoot        Document root folder.
     * @param requestHandler Request handler.
     */
    RemoteControlServer(std::string address, unsigned short port, std::string docRoot,
                        IClientRequestHandler &requestHandler);
    ~RemoteControlServer() override;

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    /**
     * @brief Waits until IO context thread has finished.
     */
    void waitUntilFinished()
    {
        if (isRunning())
        {
            m_thread.join();
        }
    }

private:
    /**
     * @brief Start listening.
     *
     * @return true If start listening succeeded.
     * @return false If start listening failed.
     */
    bool listen();

    /**
     * @brief Handles a new client event.
     *
     * @param connection Connection regarding to the event.
     * @param event Event received.
     * @param eventData Optional event data.
     */
    void handleEvent(mg_connection *connection, int event, void *eventData);

    /**
     * @brief Returns a connection object according to the client id.
     *
     * @param id Client identifier for which the connection object should be returned.
     * @return Connection reference.
     */
    Connection &getConnection(IClientRequestHandler::ClientId id)
    {
        std::lock_guard<std::mutex> lock(m_mutexStartStop);
        auto                        connection = m_connections.find(id);
        assert(connection != m_connections.end());
        return connection->second;
    }

    /**
     * @brief Sends a notification to the client.
     *
     * @param notification Notification to be sent.
     */
    void sendNotification(const common::Json &notification);

    std::string             m_address;                  ///< Servers address.
    unsigned short          m_port;                     ///< Servers port number.
    std::string             m_docRoot;                  ///< Document root.
    IClientRequestHandler & m_requestHandler;           ///< Client request handler.
    std::mutex              m_mutexStartStop;           ///< Mutex for start and stop.
    std::unique_ptr<mg_mgr> m_serverStatus;             ///< Current server status.
    common::Thread          m_thread;                   ///< Listener thread.
    bool                    m_doListen = false;         ///< Indicates if listening should be done.
    std::map<unsigned long, Connection> m_connections;  ///< List of connections to clients.
};
}  // namespace sugo::remote_control