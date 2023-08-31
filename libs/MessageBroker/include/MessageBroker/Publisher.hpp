///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <azmq/socket.hpp>
#include <string>

#include "Common/IOContext.hpp"
#include "Common/IRunnable.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/StreamBuffer.hpp"

namespace sugo::message_broker
{
/// @brief Class represents a publisher socket.
class PublisherSocket : public azmq::pub_socket
{
public:
    // TODO Use boost signle thread optimization
    explicit PublisherSocket(boost::asio::io_context& ioContext);
};

/**
 * @brief Class represents a publisher.
 *
 */
class Publisher : public common::IRunnable
{
public:
    /**
     * @brief Constructs a new publisher instance.
     *
     * @param address Publisher address.
     * @param ioContext IO context.
     */
    explicit Publisher(Address address, common::IOContext& ioContext);
    ~Publisher() override;

    /// @brief Copy constructor.
    Publisher(const Publisher&) = delete;

    /// @brief Move constructor.
    Publisher(Publisher&&) = default;

    /// @brief Copy operator.
    Publisher& operator=(const Publisher&) = delete;

    /// @brief Move operator.
    Publisher& operator=(Publisher&&) = default;

    /**
     * @brief Publish a message to a specified topic.
     *
     * @param topic   Topic to which the message should be published.
     * @param message Message to be published.
     * @return true   If the message could be published.
     * @return false  If the message could not be published.
     */
    bool publish(const Topic& topicId, const StreamBuffer& message);

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
    std::string     m_address;            ///< Publisher address.
    PublisherSocket m_socket;             ///< Publisher socket.
    bool            m_isRunning = false;  ///< Indicates if publisher is running.
};

}  // namespace sugo::message_broker
