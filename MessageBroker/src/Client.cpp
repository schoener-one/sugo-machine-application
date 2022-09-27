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
    bool success = false;
    try
    {
        LOG(trace) << "Connect to address: " << address;
        m_socket->connect(address);
        m_socket->set_option(azmq::socket::snd_timeo(static_cast<int>(timeoutSend.count())));
        m_socket->set_option(azmq::socket::rcv_timeo(static_cast<int>(timeoutReceive.count())));
        m_address = address;
        success   = true;
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to connect: " << error.what();
    }
    return success;
}

bool Client::disconnect()
{
    bool success = false;
    try
    {
        if (isConnected())
        {
            LOG(trace) << "Disconnect from address: " << m_address;
            m_socket->disconnect(m_address);
            m_address.clear();
            success = true;
        }
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << "Failed to disconnect: " << error.what();
    }
    return success;
}

bool Client::send(const StreamBuffer& outMessage, StreamBuffer& inResponse)
{
    try
    {
        auto size = m_socket->send(outMessage.data());
        LOG(trace) << "Sent " << size << " bytes";
        if (size == 0)
        {
            LOG(warning) << "No data sent";
            return false;
        }
        size = m_socket->receive(inResponse.prepare(StreamBuffer::MaxBufferSize));
        inResponse.commit(size);
    }
    catch (boost::system::system_error& error)
    {
        LOG(error) << error.what();
        return false;
    }
    return true;
}
