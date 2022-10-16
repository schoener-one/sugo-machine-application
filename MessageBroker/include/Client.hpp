///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020, Schoener-One
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <memory>

#include "IOContext.hpp"
#include "Message.hpp"
#include "StreamBuffer.hpp"

namespace sugo::message
{
class ClientSocket;

/**
 * @brief Class for sending messages.
 * @todo This class should use the azmq::message approach for sending.
 *
 */
class Client
{
public:
    explicit Client(IOContext& ioContext);
    virtual ~Client();

    /**
     * @brief Sets the send and receive timeouts.
     *
     * @param address The address to connect to.
     * @param timeoutSend Receive timeout.
     * @param timeoutReceive Send timeout.
     */
    bool connect(const Address&                   address,
                 const std::chrono::milliseconds& timeoutSend    = std::chrono::milliseconds(-1),
                 const std::chrono::milliseconds& timeoutReceive = std::chrono::milliseconds(-1));

    /**
     * Disconnects from the last connected point.
     *
     * @return true if a connection was established and could be disconnected.
     */
    bool disconnect();

    /**
     * @brief Sends a request message to the defined address and waits for the response.
     *
     * @param outMessage      Message to be sent.
     * @param[out] inResponse The response which should be replied from the server.
     * @return                True if the message could be send and the response has been received.
     */
    bool send(const StreamBuffer& outMessage, StreamBuffer& inResponse);

    /**
     * @brief Indicates if a connection is established.
     *
     * @return true  If connection is established.
     * @return false If connection is not established.
     */
    bool isConnected() const
    {
        return !m_address.empty();
    }

private:
    std::unique_ptr<ClientSocket> m_socket;
    Address                       m_address;
};

}  // namespace sugo::message
