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

namespace sugo
{
class ServerSocket : public azmq::rep_socket
{
public:
    // FIXME socket could be used optimized for single threaded!
    explicit ServerSocket(boost::asio::io_context& context) : azmq::rep_socket(context)
    {
    }
};
}  // namespace sugo

using namespace sugo;
namespace asio = boost::asio;

Server::Server(const std::string& address, IMessageHandler& messageHandler, AsioContext& ioContext)
    : m_address(address),
      m_messageHandler(messageHandler),
      m_socket(std::make_unique<ServerSocket>(ioContext.getContext())),
      m_isRunning(false)
{
}

Server::~Server()
{
    stop();
}

bool Server::start()
{
    assert(!m_isRunning);
    try
    {
        m_socket->bind(m_address);
        LOG(debug) << "bind to address: " << m_address;
        receiveRequest();
        m_isRunning = true;
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to start receiving: " << error.what();
    }
    return m_isRunning;
}

void Server::receiveRequest()
{
    m_socket->async_receive(m_receiveBuf.prepare(StreamBuffer::MaxBufferSize),
                            [&](boost::system::error_code ec, std::size_t bytesReceived) {
                                if (!ec)
                                {
                                    LOG(trace) << "Received " << bytesReceived << " bytes";
                                    if (bytesReceived > 0)
                                    {
                                        m_receiveBuf.commit(bytesReceived);
                                        (void)handleReceived();
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
                            });
}

bool Server::handleReceived()
{
    bool success = m_messageHandler.processReceived(m_receiveBuf, m_sendBuf);

    if (success && (m_sendBuf.size() > 0))
    {
        // Just send if we have something to send!
        success = sendResponse();
    }

    return success;
}

bool Server::sendResponse()
{
    bool success = false;

    try
    {
        auto const size = m_socket->send(m_sendBuf.data());
        m_sendBuf.consume(size);
        success = (size > 0);
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to send: " << error.what();
    }

    return success;
}

void Server::stop()
{
    if (isRunning())
    {
        try
        {
            m_socket->unbind(m_address);
        }
        catch (boost::system::system_error& error)
        {
            LOG(error) << "Failed unbind socket: " << error.what();
        }
        m_isRunning = false;
    }
}
