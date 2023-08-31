///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
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

#include <string>

#include "Common/IRunnable.hpp"
#include "ServiceComponent/IServiceComponent.hpp"

namespace sugo::service_component
{
/// @brief Interface class for execution bundles.
class IExecutionBundle : public common::IRunnable
{
public:
    /// @brief Default
    ~IExecutionBundle() override = default;

    /**
     * @brief Returns the id of the component.
     *
     * @return Component id
     */
    virtual std::string getId() const = 0;

    /**
     * @brief Waits until the component main process execution has finished.
     *
     */
    virtual void waitUntilFinished() = 0;

    /**
     * @brief Get the Service Component object
     *
     * @return ServiceComponent&
     */
    virtual IServiceComponent& getServiceComponent() = 0;
};
}  // namespace sugo::service_component