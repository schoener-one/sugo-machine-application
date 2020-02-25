/*
 * CommunicationService.hpp
 *
 *  Created on: 25.01.2020
 *      Author: denis
 */

#ifndef COMMUNICATIONSERVICE_HPP_
#define COMMUNICATIONSERVICE_HPP_

#include <string>
#include <memory>

#include <boost/asio.hpp>

namespace moco
{
class RepSocket;

class CommunicationService
{
public:
	constexpr static unsigned BUFFER_SIZE = 2048;

	explicit CommunicationService(const std::string& address);
	virtual ~CommunicationService();

	/**
	 * Runs the communication processing service.
	 * The method does not block, but returns after creation.
	 * @return true if succeeded.
	 */
	bool start();
	bool stop();

	/**
	 * Returns the service address.
	 * @return The service address.
	 */
	const std::string& getAddress()
	{
		return m_address;
	}

protected:
	/**
	 * Processes the received message.
	 * @param message The message which has been received.
	 * @return true if the message could be processed.
	 */
	virtual bool processMessage(const std::string& message) = 0;

	bool sendMessage(const std::string& message);

private:
	boost::asio::io_service m_ioService;
	const std::string m_address;
	std::unique_ptr<RepSocket> m_repSocket;
};

} // namespace moco

#endif /* COMMUNICATIONSERVICE_HPP_ */
