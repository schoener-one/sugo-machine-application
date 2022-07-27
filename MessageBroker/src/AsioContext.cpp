///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-25
 */
///////////////////////////////////////////////////////////////////////////////

#include "AsioContext.hpp"
#include "Logger.hpp"

using namespace sugo;

bool AsioContext::start()
{
    return m_thread.start(
        [&] {
            try
            {
                // boost::asio::executor_work_guard<decltype(m_asioContext.get_executor())> work{
                //     m_asioContext.get_executor()};

                LOG(debug) << "Starting IO context thread";
                (void)m_asioContext.run();
                LOG(debug) << "IO context thread has finished";
            }
            catch (boost::system::system_error& error)
            {
                LOG(error) << "Failed to run IO context: " << error.what();
            }
        },
        m_policy, m_priority);
}

void AsioContext::stop()
{
    LOG(debug) << "Stopping IO context thread";
    m_asioContext.stop();
    waitUntilFinished();
    m_asioContext.reset();
    LOG(debug) << "IO context thread has stopped";
}
