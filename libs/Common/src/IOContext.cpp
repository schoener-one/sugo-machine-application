///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/asio/executor_work_guard.hpp>

#include "Common/IOContext.hpp"
#include "Common/Logger.hpp"

using namespace sugo::common;

IOContext::IOContext(const std::string& instanceId, Thread::Policy policy, int priority)
    : ProcessContext(instanceId, policy, priority)
{
    setProcessRunner(
        [&] {
            try
            {
                // Keeps the run() method to return too soon!
                boost::asio::executor_work_guard<decltype(m_ioContext.get_executor())> work{
                    m_ioContext.get_executor()};

                LOG(trace) << "Starting io context";
                (void)m_ioContext.run();
                LOG(trace) << "Io context stopped";
                work.reset();
            }
            catch (boost::system::system_error& error)
            {
                LOG(error) << "Failed to run io context: " << error.what();
            }
            // Prepare for next run!
            m_ioContext.reset();
        },
        [&] { m_ioContext.stop(); });
}
