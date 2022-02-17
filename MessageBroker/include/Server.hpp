///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   25.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <array>
#include <istream>
#include <memory>

#include "IOContext.hpp"
#include "IRunnable.hpp"
#include "StreamBuffer.hpp"

namespace sugo
{
class ServerSocket;

/**
 * Class for receiving messages.
 */
class Server : public IRunnable
{
public:
    /**
     * Interface for handle received messages.
     */
    class IMessageHandler
    {
    public:
        virtual ~IMessageHandler() {}

        /**
         * Called to process a new received message.
         * @param in  Input stream buffer as received message.
         * @param out Output stream buffer to write the response to.
         * @return true if the message could be processed.
         */
        virtual bool processReceived(StreamBuffer& in, StreamBuffer& out) = 0;

    protected:
        IMessageHandler() {}
    };

    /**
     * Constructor
     * @param address Address to bind to as listener.
     * @param messageHandler Instance to handle received messages.
     * @param ioContext IO context.
     */
    Server(const std::string& address, IMessageHandler& messageHandler, IOContext& ioContext);

    virtual ~Server();

    /**
     * Returns the binded address.
     * @return Binded address
     */
    const std::string& getAddress() { return m_address; }

    // IRunnable {{
    bool start() override;

    void stop() override;

    bool isRunning() const override { return m_isRunning; }
    // IRunnable }}

private:
    bool sendResponse();
    void receiveRequest();
    bool handleReceived();

    const std::string             m_address;
    IMessageHandler&              m_messageHandler;
    std::unique_ptr<ServerSocket> m_socket;
    bool                          m_isRunning;
    StreamBuffer                  m_receiveBuf;
    StreamBuffer                  m_sendBuf;
};

}  // namespace sugo

#endif  // SERVER_HPP_
