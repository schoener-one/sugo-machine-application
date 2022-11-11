///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "Publisher.hpp"
#include "Globals.hpp"
#include "Logger.hpp"

#include <azmq/socket.hpp>
#include <boost/asio/buffer.hpp>

namespace sugo::message
{
class PublisherSocket : public azmq::pub_socket
{
public:
    explicit PublisherSocket(boost::asio::io_context& context) : azmq::pub_socket(context)
    {
    }
};
}  // namespace sugo::message

using namespace sugo::message;

Publisher::Publisher(const std::string& address, IOContext& ioContext)
    : m_address(address),
      m_socket(std::make_unique<PublisherSocket>(ioContext.getContext())),
      m_isRunning(false)
{
}

Publisher::~Publisher()
{
}

bool Publisher::publish(const MessageId& topic, const StreamBuffer& message)
{
    boost::system::error_code ec;
    // Send topic first
    size_t sentBytes = m_socket->send(boost::asio::buffer(topic), ZMQ_SNDMORE, ec);

    if (ec)
    {
        LOG(error) << "Failed to send publish topic: " << ec.message();
        return false;
    }
    else if (sentBytes == 0)
    {
        LOG(error) << "Failed to send publish topic: no bytes sent";
        return false;
    }

    // Send message data afterwards
    sentBytes = m_socket->send(message.data(), ZMQ_DONTWAIT, ec);

    if (ec)
    {
        LOG(error) << "Failed to publish message: " << ec.message();
        return false;
    }
    else if (sentBytes == 0)
    {
        LOG(error) << "Failed to send publish topic: no bytes sent";
        return false;
    }

    LOG(trace) << "Published " << sentBytes << " bytes";

    return true;
}

bool Publisher::start()
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
    return m_isRunning;
}

void Publisher::stop()
{
    if (!isRunning())
    {
        return;
    }

    boost::system::error_code ec;
    m_socket->unbind(m_address, ec);

    if (ec)
    {
        LOG(error) << "Failed unbind from address: " << ec.message();
    }

    m_isRunning = false;
}
