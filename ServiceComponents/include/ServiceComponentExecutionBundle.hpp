///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-12-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommandMessageBroker.hpp"
#include "IConfiguration.hpp"
#include "ServiceComponent.hpp"

namespace sugo
{
struct IServiceComponentExecutionBundle
{
    IServiceComponentExecutionBundle(const CommandMessageBroker::ReceiverIdType& id) : id(id)
    {
    }
    virtual AsioContext&      getContext()          = 0;
    virtual ServiceComponent& getServiceComponent() = 0;
    const std::string&        getId() const
    {
        return id;
    }

    const CommandMessageBroker::ReceiverIdType id;
};

template <class ComponentT, typename... ComponentArgs>
struct ServiceComponentExecutionBundle : IServiceComponentExecutionBundle
{
    ServiceComponentExecutionBundle(ComponentArgs&... componentArgs)
        : IServiceComponentExecutionBundle(ComponentT::ReceiverId),
          context(),
          broker(ComponentT::ReceiverId, context),
          component(broker, componentArgs...)
    {
    }

    AsioContext          context;
    CommandMessageBroker broker;
    ComponentT           component;

    AsioContext& getContext() override
    {
        return context;
    }
    ServiceComponent& getServiceComponent()
    {
        return component;
    }
};

}  // namespace sugo