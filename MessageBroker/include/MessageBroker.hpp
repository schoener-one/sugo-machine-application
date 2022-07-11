///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGEBROKER_HPP_
#define MESSAGEBROKER_HPP_

#include <map>

#include "Client.hpp"
#include "Globals.hpp"
#include "IMessageBroker.hpp"
#include "Server.hpp"

namespace sugo
{
template <class MessageT, class ResponseT = void, class ReceiverIdT = unsigned int>
class MessageBroker : public IMessageBroker<MessageT, ResponseT, ReceiverIdT>
{
public:
    using typename IMessageBroker<MessageT, ResponseT, ReceiverIdT>::Handler;

    // IMessageBroker {{
    void registerHandler(const ReceiverIdT& receiverId, Handler& handler) override
    {
        m_handlers[receiverId] = handler;
    }

    void registerHandler(const ReceiverIdT& receiverId, Handler&& handler) override
    {
        m_handlers[receiverId] = handler;
    }

    void unregisterHandler(const ReceiverIdT& receiverId) override
    {
        auto iter = m_handlers.find(receiverId);
        if (iter != m_handlers.end())
        {
            m_handlers.erase(iter);
        }
    }

    bool hasRegisteredHandler(const ReceiverIdT& receiverId) const
    {
        auto iter = m_handlers.find(receiverId);
        return (iter != m_handlers.end());
    }
    // IMessageBroker }}

protected:
    Handler* findHandler(const ReceiverIdT& receiverId)
    {
        Handler* handler = nullptr;
        auto     iter    = m_handlers.find(receiverId);
        if (iter != m_handlers.end())
        {
            handler = &(iter->second);
        }
        return handler;
    }

private:
    using MessageReceiverMap = std::map<ReceiverIdT, Handler>;

    MessageReceiverMap m_handlers;
};

}  // namespace sugo

#endif /* MESSAGEBROKER_HPP_ */
