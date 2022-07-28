///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-12-01
 */
///////////////////////////////////////////////////////////////////////////////

#include "ComponentsExecutionGroup.hpp"
#include "CommandMessageBroker.hpp"
#include "ServiceComponent.hpp"

#include <memory>

#include "FilamentCoilControl.hpp"
#include "FilamentCoilMotor.hpp"
#include "FilamentFeederMotor.hpp"
#include "FilamentMergerControl.hpp"
#include "FilamentMergerHeater.hpp"
#include "FilamentPreHeater.hpp"
#include "FilamentTensionSensor.hpp"
#include "MachineControl.hpp"
#include "MachineServiceGateway.hpp"
#include "UserInterfaceControl.hpp"

namespace sugo
{
struct IComponentExecutionBundle
{
    IComponentExecutionBundle(const CommandMessageBroker::ReceiverIdType& id) : id(id)
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
struct ComponentExecutionBundle : IComponentExecutionBundle
{
    ComponentExecutionBundle(const CommandMessageBroker::ReceiverIdType& receiverId,
                             ComponentArgs&... componentArgs)
        : IComponentExecutionBundle(receiverId),
          context(),
          broker(receiverId, context),
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

struct MachineServiceGatewayExecutionBundle : IComponentExecutionBundle
{
    MachineServiceGatewayExecutionBundle(const CommandMessageBroker::ReceiverIdType& receiverId,
                                         const IConfiguration&                       configuration)
        : IComponentExecutionBundle(receiverId),
          context(),
          broker(receiverId, context),
          component(broker, configuration, context)
    {
    }

    AsioContext           context;
    CommandMessageBroker  broker;
    MachineServiceGateway component;

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

using namespace sugo;

void ComponentsExecutionGroup::addConfigurationOptions(IConfiguration& configuration)
{
    MachineServiceGateway::addConfigurationOptions(configuration);
}

bool ComponentsExecutionGroup::start(const ServiceLocator& serviceLocator)
{
    // Gateway
    MachineServiceGatewayExecutionBundle machineServiceGateway(
        MachineServiceGateway::ReceiverId, serviceLocator.get<IConfiguration>());

    // Service components
    ComponentExecutionBundle<MachineControl> machineControl(MachineControl::ReceiverId);

    std::array<IComponentExecutionBundle*, 2> bundles{&machineControl, &machineServiceGateway};

    for (auto const& bundle : bundles)
    {
        if (!bundle->getServiceComponent().start())
        {
            LOG(error) << "Failed to start component " << bundle->getId();
            return false;
        }
    }

    // Now wait until all have finished
    for (auto const& bundle : bundles)
    {
        bundle->getContext().waitUntilFinished();
    }
    return true;
}
