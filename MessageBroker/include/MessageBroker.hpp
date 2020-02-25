/*
 * MessageBroker.hpp
 *
 *  Created on: 02.11.2019
 *      Author: denis
 */

#ifndef MESSAGEBROKER_HPP_
#define MESSAGEBROKER_HPP_

#include <map>
#include <cassert>

#include "Globals.hpp"
#include "IMessageBroker.hpp"

namespace moco
{

template<class MessageT, class ResponseT = void, class ReceiverIdT = unsigned int>
class MessageBroker : public IMessageBroker<MessageT, ResponseT, ReceiverIdT>
{
public:
	using typename IMessageBroker<MessageT, ResponseT, ReceiverIdT>::Receiver;

	// IMessageBroker {{
	bool hasReceiver(const ReceiverIdT& receiverId) const override
	{
		return m_receivers.count(receiverId) > 0;
	}

	void notify(const MessageT& message, const ReceiverIdT& senderId) override
	{
		//TODO use a multimap for multiple receivers here!
		LOG(debug) << "Sending message to " << senderId;
		if (hasReceiver(senderId))
		{
			m_receivers[senderId](message);
		}
	}

	ResponseT send(const MessageT& message, const ReceiverIdT& receiverId) override
	{
		LOG(debug) << "Sending message to " << receiverId;
		assert(hasReceiver(receiverId));
		return m_receivers[receiverId](message);
	}

	void registerReceiver(const ReceiverIdT& receiverId, Receiver& receiver) override
	{
		m_receivers[receiverId] = receiver;
	}

	void registerReceiver(const ReceiverIdT& receiverId, Receiver&& receiver) override
	{
		m_receivers[receiverId] = receiver;
	}

	void unregisterReceiver(const ReceiverIdT& receiverId) override
	{
		auto iter = m_receivers.find(receiverId);
		if (iter != m_receivers.end())
		{
			m_receivers.erase(iter);
		}
	}
	// IMessageBroker }}

private:
	using ReceiverMap = std::map<ReceiverIdT, Receiver>;
	ReceiverMap m_receivers;
};

} // namespace moco

#endif /* MESSAGEBROKER_HPP_ */
