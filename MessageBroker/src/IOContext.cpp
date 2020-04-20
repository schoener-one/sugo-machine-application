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

bool IOContext::start(Thread::Policy policy, int priority)
{
    return m_thread.start(
        [=] {
            try
            {
                run();
            }
            catch (boost::system::system_error& error)
            {
                LOG(error) << "Failed to run asynchronous IO: " << error.what();
            }
        },
        policy, priority);
}
