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
    virtual AsioContext&      getContext()          = 0;
    virtual ServiceComponent& getServiceComponent() = 0;
};

template <class ComponentT, typename... ComponentArgs>
struct ComponentExecutionBundle : IComponentExecutionBundle
{
    ComponentExecutionBundle(const CommandMessageBroker::ReceiverIdType& receiverId,
                             ComponentArgs&... componentArgs)
        : context(), broker(receiverId, context), component(broker, componentArgs...)
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
        : context(), broker(receiverId, context), component(broker, configuration, context)
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
    ComponentExecutionBundle<MachineControl> machineControl(MachineControl::ReceiverId);

    // Gateway
    MachineServiceGatewayExecutionBundle machineServiceGateway(MachineServiceGateway::ReceiverId,
                                                               serviceLocator.get<IConfiguration>());

    std::array<IComponentExecutionBundle*, 2> bundles{&machineControl, &machineServiceGateway};

    for (auto const& bundle : bundles)
    {
        if (!bundle->getServiceComponent().start())
        {
            LOG(error) << "Failed to start component " << bundle->getServiceComponent().getId();
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
