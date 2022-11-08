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
    static const unsigned MaxConnections = 3u;

    RemoteControlServer(const std::string address, unsigned short port, const std::string docRoot,
                        IRequestHandler &requestHandler);
    ~RemoteControlServer();

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