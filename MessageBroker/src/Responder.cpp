///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   25.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "Responder.hpp"

#include <azmq/socket.hpp>

#include "Globals.hpp"

namespace moco
{
class ResponderSocket : public azmq::rep_socket
{
public:
    explicit ResponderSocket(IOContext& service) : azmq::rep_socket(service) {}
};

}  // namespace moco

using namespace moco;
namespace asio = boost::asio;

Responder::Responder(const std::string& address, IMessageHandler& messageHandler,
                     IOContext& ioContext)
    : m_address(address),
      m_messageHandler(messageHandler),
      m_socket(std::make_unique<ResponderSocket>(ioContext)),
      m_isRunning(false)
{
}

Responder::~Responder() { stop(); }

bool Responder::start()
{
    assert(!m_isRunning);
    try
    {
        m_socket->bind(m_address);
        receiveRequest();
        m_isRunning = true;
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to start receiving: " << error.what();
    }
    return m_isRunning;
}

void Responder::receiveRequest()
{
    m_socket->async_receive(m_receiveBuf.prepare(StreamBuffer::MaxBufferSize),
                            [&](boost::system::error_code ec, std::size_t bytesReceived) {
                                LOG(debug) << "Received " << bytesReceived << " bytes";
                                if (!ec)
                                {
                                    if (bytesReceived > 0)
                                    {
                                        m_receiveBuf.commit(bytesReceived);
                                        m_isRunning = handleReceived();
                                    }
                                    else
                                    {
                                        LOG(warning) << "Received empty buffer";
                                    }
                                }
                                else
                                {
                                    LOG(error) << "Receive error occurred: " << ec;
                                }

                                if (m_isRunning)
                                {
                                    receiveRequest();
                                }
                                else
                                {
                                    LOG(debug) << "Receiving data stopped";
                                }
                            });
}

bool Responder::handleReceived()
{
    bool success = m_messageHandler.processReceived(m_receiveBuf, m_sendBuf);

    if (success)
    {
        success = sendResponse();
    }

    return success;
}

bool Responder::sendResponse()
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

void Responder::stop()
{
    if (m_isRunning)
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
