/*
 * CommunicationService.cpp
 *
 *  Created on: 25.01.2020
 *      Author: denis
 */

#include "Globals.hpp"
#include "CommunicationService.hpp"

#include <azmq/socket.hpp>

namespace moco
{

class RepSocket : public azmq::rep_socket
{
public:
	explicit RepSocket(boost::asio::io_service& service)
		:azmq::rep_socket(service) {}
};

}
using namespace moco;
namespace asio = boost::asio;

CommunicationService::CommunicationService()
:m_ioService()
{
}

CommunicationService::~CommunicationService()
{
}

bool CommunicationService::start(const std::string& address)
{
	try
	{
		m_repSocket = std::make_unique<RepSocket>(m_ioService);
		m_repSocket->bind(address);
		std::array<char, BUFFER_SIZE> buf;

		while(true)
		{
			auto size = m_repSocket->receive(asio::buffer(buf));
			const std::string message(buf.data(), size);
			LOG(debug) << "Request of " << size << " bytes received";
			(void)processMessage(message);
		}
	}
	catch(boost::system::system_error& error)
	{
		LOG(error) << error.what();
	}
	return true;
}

// cppcheck-suppress unusedFunction
bool CommunicationService::stop()
{
	assert(m_repSocket);
	return true;
}

bool CommunicationService::sendMessage(const std::string& message)
{
	assert(m_repSocket);
	LOG(debug) << "Sending message: '" << message << "'";
	return m_repSocket->send(asio::buffer(message)) > 0;
}

