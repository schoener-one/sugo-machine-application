///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   22.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ICommandMessageBroker.hpp"
#include "IRunnable.hpp"
#include "Message.hpp"

#include <vector>

namespace sugo
{
/**
 * Interface class for service components.
 *
 */
class IServiceComponent : public IRunnable
{
public:
    /// @brief List of notification ids.
    using NotificationIdList = std::vector<message::NotificationId>;

    ~IServiceComponent() override = default;

    /**
     * @brief Returns the message broker.
     *
     * @return The message broker.
     */
    virtual message::ICommandMessageBroker& getCommandMessageBroker() = 0;
};
}  // namespace sugo
