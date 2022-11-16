///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   25.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "Server.hpp"
#include "Globals.hpp"

#include <azmq/socket.hpp>

namespace sugo::message
{
class ServerSocket : public azmq::rep_socket
{
public:
    // FIXME socket could be used optimized for single threaded!
    explicit ServerSocket(boost::asio::io_context& context) : azmq::rep_socket(context)
    {
    }
};
}  // namespace sugo::message

using namespace sugo::message;
namespace asio = boost::asio;

Server::Server(std::string address, IMessageHandler& messageHandler, IOContext& ioContext)
    : m_address(std::move(address)),
      m_messageHandler(messageHandler),
      m_socket(std::make_unique<ServerSocket>(ioContext.getContext()))
{
}

Server::~Server()  // NOLINT(modernize-use-equals-default) - std::unique_ptr/std::default_delete
                   // sizeof unknown
{
}

bool Server::start()
{
    assert(!m_isRunning);
    boost::system::error_code ec;
    m_socket->bind(m_address, ec);

    if (ec)
    {
        LOG(error) << "Failed to bind address: " << ec.message();
        return false;
    }

    LOG(debug) << "Bind to address: " << m_address;
    m_isRunning = true;
    receiveRequest();
    return m_isRunning;
}

void Server::receiveRequest()
{
    m_socket->async_receive(
        m_receiveBuffer.prepare(StreamBuffer::MaxBufferSize),
        [&](boost::system::error_code ec, std::size_t bytesReceived) {
            LOG(trace) << "Received " << bytesReceived << " bytes";

            if (!ec)
            {
                if (bytesReceived > 0)
                {
                    m_receiveBuffer.commit(bytesReceived);
                    (void)handleReceived(m_receiveBuffer);
                }
                else
                {
                    LOG(warning) << "Received empty buffer";
                }
            }
            else
            {
                LOG(error) << "Receive error occurred: " << ec.message();
            }

            if (isRunning())
            {
                receiveRequest();
            }
        },
        ZMQ_DONTWAIT);
}

bool Server::handleReceived(StreamBuffer& receiveBuf)
{
    StreamBuffer sendBuffer;
    bool         success = m_messageHandler.processReceived(receiveBuf, sendBuffer);

    if ((sendBuffer.size() > 0) && success)
    {
        // Just send if we have something to send!
        success = sendResponse(sendBuffer);
    }
    else
    {
        LOG(error) << "Failed to process received message"
                   << (((sendBuffer.size() == 0) ? ": response message is empty" : ""));
        success = false;
    }

    // Do a post process here to decouple message response from internal processing!
    m_messageHandler.processPost();
    return success;
}

bool Server::sendResponse(const StreamBuffer& sendBuffer)
{
    boost::system::error_code ec;
    auto const                size = m_socket->send(sendBuffer.data(), 0, ec);

    if (ec || (size == 0))
    {
        LOG(error) << "Failed to send message: "
                   << (ec.failed() ? ec.message() : "response could not be sent");
        return false;
    }

    return true;
}

void Server::stop()
{
    if (isRunning())
    {
        boost::system::error_code ec;
        m_socket->unbind(m_address, ec);

        if (ec)
        {
            LOG(error) << "Failed unbind from address: " << ec.message();
        }

        m_isRunning = false;
    }
}
