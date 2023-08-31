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
#include <vector>

#include "Common/IRunnable.hpp"
#include "MessageBroker/IMessageBroker.hpp"
#include "MessageBroker/Message.hpp"
#include "ServiceComponent/ServiceId.hpp"

namespace sugo::service_component
{
/// @brief Interface class for service components.
class IServiceComponent : public common::IRunnable
{
public:
    /// @brief Service component identifier type.
    using Identifier = const char*;

    /// @brief List of notification ids.
    using NotificationIdList = std::vector<NotificationId>;

    ~IServiceComponent() override = default;

    /**
     * @brief Returns the message broker.
     *
     * @return The message broker.
     */
    virtual message_broker::IMessageBroker& getMessageBroker() = 0;
};
}  // namespace sugo::service_component
