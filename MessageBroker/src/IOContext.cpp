///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   26.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#include "IOContext.hpp"
#include "Logger.hpp"

#include <boost/asio/executor_work_guard.hpp>

using namespace sugo;

IOContext::IOContext(const std::string& instanceId, Thread::Policy policy, int priority)
    : ProcessContext(instanceId, policy, priority)
{
    setProcessRunner(
        [&] {
            try
            {
                // Keeps the run() method to return too soon!
                boost::asio::executor_work_guard<decltype(m_asioContext.get_executor())> work{
                    m_asioContext.get_executor()};

                LOG(trace) << "Starting io context";
                (void)m_asioContext.run();
                LOG(trace) << "Io context stopped";
                work.reset();
            }
            catch (boost::system::system_error& error)
            {
                LOG(error) << "Failed to run io context: " << error.what();
            }
            // Prepare for next run!
            m_asioContext.reset();
        },
        [&] { m_asioContext.stop(); });
}
