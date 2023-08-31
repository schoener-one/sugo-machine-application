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

#pragma once

#include "Common/IRunnable.hpp"
#include "Common/Thread.hpp"

namespace sugo::common
{
/// @brief Interface class representing a process context.
class IProcessContext : public IRunnable
{
public:
    /// @brief Default destructor
    ~IProcessContext() override = default;

    /**
     * @brief Set the process runner function to be used on start from this context.
     * Must not be set during running!
     *
     * @param process     Process to be used.
     * @param stopProcess Process to be called in order to stop the process.
     * @return true       If the process has not yet started and could be set.
     * @return false      If the process has already started.
     */
    virtual bool setProcessRunner(Thread::Runnable process, Thread::Runnable stopProcess) = 0;

    /**
     * Waits until process thread has finished.
     */
    virtual void waitUntilFinished() = 0;
};

}  // namespace sugo::common
