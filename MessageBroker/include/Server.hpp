///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020, Schoener-One
 *
 * @author: denis
 * @date:   25.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <istream>
#include <memory>

#include "IOContext.hpp"
#include "IRunnable.hpp"
#include "StreamBuffer.hpp"
#include "IMessageHandler.hpp"
#include "Message.hpp"

namespace sugo::message
{
class ServerSocket;

/**
 * Class for receiving messages.
 */
class Server : public IRunnable
{
public:
    /**
     * Constructor
     * @param address Address to bind to as listener.
     * @param messageHandler Instance to handle received messages.
     * @param ioContext IO context.
     */
    Server(const Address& address, IMessageHandler& messageHandler, IOContext& ioContext);

    virtual ~Server();

    /**
     * Returns the binded address.
     * @return Binded address
     */
    const Address& getAddress()
    {
        return m_address;
    }

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_isRunning;
    }

private:
    bool sendResponse(const StreamBuffer& sendBuffer);
    void receiveRequest();
    bool handleReceived(StreamBuffer& receiveBuf);

    const std::string             m_address;
    IMessageHandler&              m_messageHandler;
    std::unique_ptr<ServerSocket> m_socket;
    bool                          m_isRunning = false;
    StreamBuffer                  m_receiveBuffer;
};

}  // namespace sugo
