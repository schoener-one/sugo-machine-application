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

namespace sugo::common
{
/// @brief Interface class for runnable classes.
class IRunnable
{
public:
    /// @brief Default destructor
    virtual ~IRunnable() = default;

    /**
     * Starts the runnable instance.
     * @return true if the instance could be started successfully.
     */
    virtual bool start() = 0;

    /**
     * Stops the runnable instance.
     */
    virtual void stop() = 0;

    /**
     * Indicates if the instance is still running.
     * @return true if the instance is still running.
     */
    virtual bool isRunning() const = 0;
};
}  // namespace sugo::common
