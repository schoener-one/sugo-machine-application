///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   06.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IOCONTEXTHELPER_HPP_
#define IOCONTEXTHELPER_HPP_

#include <boost/asio.hpp>
#include <thread>

#include "Logger.hpp"
#include "Responder.hpp"

namespace moco
{
class IOContextHelper
{
public:
    void startIOContext()
    {
        m_contextThread = std::make_shared<std::thread>([&] {
            moco::Logger::init(Logger::Severity::error);
            m_ioContext.run();
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void stopIOContext()
    {
        if (!m_ioContext.stopped())
        {
            m_ioContext.stop();
        }
        if (m_contextThread)
        {
            m_contextThread->join();
            m_contextThread.reset();
        }
    }

    moco::IOContext              m_ioContext;
    std::shared_ptr<std::thread> m_contextThread;
};

}  // namespace moco

#endif  // IOCONTEXTHELPER_HPP_
