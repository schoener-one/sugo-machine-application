///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IMESSAGEBROKER_HPP_
#define IMESSAGEBROKER_HPP_

#include <chrono>
#include <functional>
#include <vector>

#include "IRunnable.hpp"
#include "Thread.hpp"

namespace sugo
{
/**
 * Interface class to handle messaging between components.
 * @tparam MessageT Message type.
 * @tparam ResponseT Message response type.
 * @tparam ReceiverIdT Receiver identifier type.
 */
template <class MessageT, class ResponseT = void, class ReceiverIdT = unsigned int>
class IMessageBroker : public IRunnable
{
protected:
    virtual ~IMessageBroker() = default;

public:
    /// @brief Maximum time to transmit a message (inclusive response).
    inline static constexpr std::chrono::milliseconds MaxMessageTransmissionTime{10};
    /// @brief Address prefix
    inline static const std::string AddressPrefix{"inproc://"};
    /// @brief Maximum of retries for a failed transmission.
    static constexpr unsigned MaxMessageTransmissionRetries = 3;

    /// Handler definition which reacts to a request.
    using Handler = std::function<ResponseT(const MessageT&)>;
    /// Type for receiver identifiers
    using ReceiverIdType = ReceiverIdT;
    /// Receiver list definition.
    using ReceiverIdList = std::vector<ReceiverIdT>;

    /**
     * Notifies all receiver instances.
     * @param message Notification message.
     * @param receivers List of receivers to be notified
     * @return true if the notification could be sent.
     */
    virtual bool notify(const MessageT& message, const ReceiverIdList& receivers) = 0;

    /**
     * Sends a synchronous request message to a receiver instance and returns there response.
     * @param message Send message.
     * @param receiverId Receiver id which should receive the message.
     * @param response Response of the request.
     * @return true if the request could be sent.
     */
    virtual bool send(const MessageT& message, const ReceiverIdT& receiverId,
                      ResponseT& response) = 0;

    /**
     * Registers a new message handler.
     * @param receiverId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const ReceiverIdT& messageId, Handler& handler) = 0;

    /**
     * Registers a new message handler.
     * @param receiverId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const ReceiverIdT& messageId, Handler&& handler) = 0;

    /**
     * Unregisters an existing message handler.
     * @param messageId Message identifier.
     */
    virtual void unregisterHandler(const ReceiverIdT& messageId) = 0;

    /**
     * Indicates if a handler has been registered for a message id.
     * @param messageId Message identifier.
     * @return true if the handler could be registered.
     */
    virtual bool hasRegisteredHandler(const ReceiverIdT& messageId) const = 0;

    /**
     * @brief Registers a post process handler which will be called if a received message
     * has been handled.
     *
     * @param postProcess Callback function to be called for post processing.
     */
    virtual void registerPostProcessHandler(Thread::Runnable postProcess) = 0;
};

}  // namespace sugo

#endif /* IMESSAGEBROKER_HPP_ */
