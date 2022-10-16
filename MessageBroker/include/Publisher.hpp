///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020, Schoener-One
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IOContext.hpp"
#include "IRunnable.hpp"
#include "Message.hpp"
#include "StreamBuffer.hpp"

#include <string>

namespace sugo::message
{
class PublisherSocket;

/**
 * @brief Class represents a publisher.
 *
 */
class Publisher : public IRunnable
{
public:
    explicit Publisher(const Address& address, IOContext& ioContext);
    virtual ~Publisher();

    /**
     * @brief Publish a message to a specified topic.
     *
     * @param topic   Topic to which the message should be published.
     * @param message Message to be published.
     * @return true   If the message could be published.
     * @return false  If the message could not be published.
     */
    bool publish(const MessageId& topic, const StreamBuffer& message);

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_isRunning;
    }

    /**
     * Returns the binded address.
     * @return Binded address
     */
    const Address& getAddress()
    {
        return m_address;
    }
private:
    std::string                      m_address;
    std::unique_ptr<PublisherSocket> m_socket;
    bool                             m_isRunning = false;
};

}  // namespace sugo::message
