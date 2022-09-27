///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   29.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ICommandMessageBroker.hpp"
#include "IRunnable.hpp"

namespace sugo
{
/**
 * Class to receive command messages and to translate them to events.
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 *
 * @todo Needs to be transformed to a base class of all ServiceComponents in order
 *       to handle messages in a generic manner!
 */
class IServiceComponent : public IRunnable
{
public:
    virtual ~IServiceComponent() = default;

    /**
     * @brief Gets the receiver list.
     * 
     * @return The receiver list.
     */
    virtual const ICommandMessageBroker::ReceiverIdList& getReceiverList() const = 0;

    /**
     * @brief Sets the a new receiver list.
     *
     * @param receiverIdList The new receiver list to set.
     */
    virtual void setReceiverList(const ICommandMessageBroker::ReceiverIdList& receiverIdList) = 0;
};

}  // namespace sugo
