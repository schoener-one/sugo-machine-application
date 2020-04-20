///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef REQUESTER_HPP_
#define REQUESTER_HPP_

#include <chrono>
#include <istream>
#include <memory>
#include <ostream>

#include "IOContext.hpp"
#include "StreamBuffer.hpp"

namespace moco
{
class RequesterSocket;

/**
 * Class for sending messages.
 */
class Requester
{
public:
    explicit Requester(IOContext& ioContext);
    virtual ~Requester();

    /**
     * Sets the send and receive timeouts.
     * @param address The address to connect to.
     * @param timeoutSend Receive timeout.
     * @param timeoutReceive Send timeout.
     */
    bool connect(const std::string&               address,
                 const std::chrono::milliseconds& timeoutSend    = std::chrono::milliseconds(-1),
                 const std::chrono::milliseconds& timeoutReceive = std::chrono::milliseconds(-1));

    /**
     * Disconnects from the last connected point.
     * @param address The address to disconnect from.
     * @return true if a connection was established and could be disconnected.
     */
    bool disconnect(const std::string& address);

    /**
     * Sends a request message to the defined address and waits for the response.
     * @param outMessage      Message to be sent.
     * @param[out] inResponse The response which should be replied from the responder.
     * @return                True if the message could be send and the response has been received.
     */
    bool send(const StreamBuffer& outMessage, StreamBuffer& inResponse);

private:
    std::unique_ptr<RequesterSocket> m_socket;
};

}  // namespace moco

#endif  // REQUESTER_HPP_
