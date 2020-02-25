/*
 * IMessageBroker.hpp
 *
 *  Created on: 02.01.2020
 *      Author: denis
 */

#ifndef IMESSAGEBROKER_HPP_
#define IMESSAGEBROKER_HPP_

#include <functional>

namespace moco
{

template<class MessageT, class ResponseT = void, class ReceiverIdT = unsigned int>
class IMessageBroker
{
protected:
	virtual ~IMessageBroker() {}

public:
	using Receiver = std::function<ResponseT (const MessageT&)>;

	virtual bool hasReceiver(const ReceiverIdT& receiverId) const = 0;

	virtual void notify(const MessageT& message, const ReceiverIdT& senderId) = 0;

	virtual ResponseT send(const MessageT& message, const ReceiverIdT& receiverId) = 0;

	virtual void registerReceiver(const ReceiverIdT& receiverId, Receiver& receiver) = 0;

	virtual void registerReceiver(const ReceiverIdT& receiverId, Receiver&& receiver) = 0;

	virtual void unregisterReceiver(const ReceiverIdT& receiverId) = 0;
};

} // namespace moco

#endif /* IMESSAGEBROKER_HPP_ */
