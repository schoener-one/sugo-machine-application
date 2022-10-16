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

namespace sugo::message
{
class ClientSocket : public azmq::req_socket
{
public:
    explicit ClientSocket(boost::asio::io_context& context) : azmq::req_socket(context)
    {
    }
};
}  // namespace sugo::message

using namespace sugo::message;
namespace asio = boost::asio;

Client::Client(IOContext& ioContext)
    : m_socket(std::make_unique<ClientSocket>(
          ioContext.getContext()))  // TODO remove dynamic allocation!
{
}

Client::~Client()
{
}

bool Client::connect(const std::string& address, const std::chrono::milliseconds& timeoutSend,
                     const std::chrono::milliseconds& timeoutReceive)
{
    boost::system::error_code ec;
    LOG(trace) << "Connecting to address: " << address;
    m_socket->connect(address, ec);

    if (ec)
    {
        LOG(error) << "Failed to connect: " << ec.message();
        return false;
    }

    m_socket->set_option(azmq::socket::snd_timeo(static_cast<int>(timeoutSend.count())));
    m_socket->set_option(azmq::socket::rcv_timeo(static_cast<int>(timeoutReceive.count())));
    m_address = address;
    return true;
}

bool Client::disconnect()
{
    boost::system::error_code ec;

    if (!isConnected())
    {
        return false;
    }

    LOG(trace) << "Disconnecting from address: " << m_address;
    m_socket->disconnect(m_address, ec);
    m_address.clear();

    if (ec)
    {
        LOG(error) << "Failed to disconnect: " << ec.message();
        return false;
    }

    return true;
}

bool Client::send(const StreamBuffer& outMessage, StreamBuffer& inResponse)
{
    boost::system::error_code ec;
    auto                      size = m_socket->send(outMessage.data(), 0, ec);

    if (ec)
    {
        LOG(error) << "Failed to send message: " << ec.message();
        return false;
    }

    LOG(trace) << "Sent " << size << " bytes";

    if (size == 0)
    {
        LOG(warning) << "No data sent";
        return false;
    }

    size = m_socket->receive(inResponse.prepare(StreamBuffer::MaxBufferSize), 0, ec);

    if (ec)
    {
        LOG(error) << "Failed to receive response: " << ec.message();
        return false;
    }

    inResponse.commit(size);
    return true;
}
