///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-30
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Connection.hpp"
#include "Globals.hpp"
#include "IConfiguration.hpp"
#include "IRequestHandler.hpp"
#include "IRunnable.hpp"
#include "Thread.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <mutex>
#include <string>

struct mg_mgr;

namespace sugo::remote_control
{
/**
 * @brief Class to handle HTTP and Websocket requests.
 *
 */
class RemoteControlServer : public IRunnable
{
public:
    /// @brief Maximum connection allowed to connect.
    static const unsigned MaxConnections = 3u;

    /**
     * @brief Set the Configuration options.
     *
     * @param configuration Configuration object to which the options will be added.
     */
    static void addConfigurationOptions(IConfiguration &configuration);

    /**
     * @brief Construct a new remove control server.
     *
     * @param configuration  Configuration for this server.
     * @param requestHandler Request handler.
     */
    RemoteControlServer(const IConfiguration &configuration, IRequestHandler &requestHandler);
    /**
     * @brief Construct a new remove control server.
     *
     * @param address        Network interface address of the server for listening.
     * @param port           Port on which the service is provided.
     * @param docRoot        Document root folder.
     * @param requestHandler Request handler.
     */
    RemoteControlServer(std::string address, unsigned short port, std::string docRoot,
                        IRequestHandler &requestHandler);
    ~RemoteControlServer() override;

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    /**
     * Waits until IO context thread has finished.
     */
    void waitUntilFinished()
    {
        if (isRunning())
        {
            m_thread.join();
        }
    }

private:
    /// Request method type
    enum RequestMethod
    {
        Unknown = 0,
        Get,
        Post,
        Update
    };

    // Helper methods
    bool        listen();
    void        handleEvent(mg_connection *connection, int event, void *eventData);
    Connection &getConnection(Connection::Identifier id)
    {
        std::lock_guard<std::mutex> lock(m_mutexStartStop);
        auto                        connection = m_connections.find(id);
        assert(connection != m_connections.end());
        return connection->second;
    }
    void sendNotification(const Json &notification);

    std::string                         m_address;
    unsigned short                      m_port;
    std::string                         m_docRoot;
    IRequestHandler &                   m_requestHandler;
    std::mutex                          m_mutexStartStop;
    std::unique_ptr<mg_mgr>             m_serverStatus;
    Thread                              m_thread;
    bool                                m_doListen = false;
    std::map<unsigned long, Connection> m_connections;
};
}  // namespace sugo::remote_control