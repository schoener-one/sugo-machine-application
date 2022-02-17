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
#include <mutex>
#include <thread>

#include "Logger.hpp"
#include "Server.hpp"

namespace sugo
{
class IOContextHelper
{
protected:
    void startIOContext()
    {
        m_contextThread = std::make_shared<std::thread>([&] {
            sugo::Logger::init(Logger::Severity::error);
            while (!m_ioContext.stopped())
            {
                std::lock_guard<std::mutex> lock(m_ioContextMutex);
                m_ioContext.run_one();
            }
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

    std::mutex                   m_ioContextMutex;
    sugo::IOContext              m_ioContext;
    std::shared_ptr<std::thread> m_contextThread;
};

}  // namespace sugo

#endif  // IOCONTEXTHELPER_HPP_
