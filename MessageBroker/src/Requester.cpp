///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <azmq/socket.hpp>
#include <chrono>

#include "Globals.hpp"
#include "Requester.hpp"

namespace moco
{
class RequesterSocket : public azmq::req_socket
{
public:
    explicit RequesterSocket(IOContext& ioContext) : azmq::req_socket(ioContext) {}
};
}  // namespace moco

using namespace moco;
namespace asio = boost::asio;

Requester::Requester(IOContext& ioContext) : m_socket(std::make_unique<RequesterSocket>(ioContext))
{
}

Requester::~Requester() {}

bool Requester::connect(const std::string& address, const std::chrono::milliseconds& timeoutSend,
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

bool Requester::disconnect(const std::string& address)
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

bool Requester::send(const StreamBuffer& outMessage, StreamBuffer& inResponse)
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
