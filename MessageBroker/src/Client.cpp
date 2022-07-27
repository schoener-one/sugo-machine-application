///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "Client.hpp"
#include "Globals.hpp"

#include <azmq/socket.hpp>
#include <chrono>

namespace sugo
{
class ClientSocket : public azmq::req_socket
{
public:
    explicit ClientSocket(boost::asio::io_context& context) : azmq::req_socket(context)
    {
    }
};
}  // namespace sugo

using namespace sugo;
namespace asio = boost::asio;

Client::Client(AsioContext& ioContext)
    : m_socket(std::make_unique<ClientSocket>(ioContext.getContext()))
{
}

Client::~Client()
{
}

bool Client::connect(const std::string& address, const std::chrono::milliseconds& timeoutSend,
                     const std::chrono::milliseconds& timeoutReceive)
{
    bool success = false;
    try
    {
        m_socket->connect(address);
        m_socket->set_option(azmq::socket::snd_timeo(static_cast<int>(timeoutSend.count())));
        m_socket->set_option(azmq::socket::rcv_timeo(static_cast<int>(timeoutReceive.count())));
        success = true;
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to connect: " << error.what();
    }
    return success;
}

bool Client::disconnect(const std::string& address)
{
    bool success = false;
    try
    {
        m_socket->disconnect(address);
        success = true;
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to disconnect: " << error.what();
    }
    return success;
}

bool Client::send(const StreamBuffer& outMessage, StreamBuffer& inResponse)
{
    bool success = false;
    try
    {
        auto size = m_socket->send(outMessage.data());
        if (size > 0)
        {
            size = m_socket->receive(inResponse.prepare(StreamBuffer::MaxBufferSize));
            inResponse.commit(size);
            success = true;
        }
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << error.what();
    }
    return success;
}

bool Client::notify(const StreamBuffer& outMessage)
{
    bool success = false;
    try
    {
        // TODO Use async_send here!
        const auto size = m_socket->send(outMessage.data());
        success         = (size > 0);
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << error.what();
    }
    return success;
}
