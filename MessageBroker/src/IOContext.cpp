///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   09.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "IOContext.hpp"
#include "Logger.hpp"

using namespace moco;

IOContext::~IOContext()
{
    if (isRunning())
    {
        stop();
    }
}

bool IOContext::start()
{
    reset();
    return m_thread.start(
        [=] {
            try
            {
                (void)run();
                LOG(debug) << "IO context has finished";
            }
            catch (boost::system::system_error& error)
            {
                LOG(error) << "Failed to run IO context: " << error.what();
            }
        },
        m_policy, m_priority);
}

void IOContext::stop()
{
    boost::asio::io_service::stop();
    waitUntilFinished();
}
