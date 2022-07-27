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

template <class ComponentT>
struct ComponentExecutionBundle : IComponentExecutionBundle
{
    ComponentExecutionBundle(const CommandMessageBroker::ReceiverIdType& receiverId)
        : context(), broker(receiverId, context), component(broker)
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
                                         Configuration&                              configuration)
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

void ComponentsExecutionGroup::addConfigurationOptions(Configuration& configuration)
{
    MachineServiceGateway::addConfigurationOptions(configuration);
}

bool ComponentsExecutionGroup::start(Configuration& configuration)
{
    ComponentExecutionBundle<MachineControl> machineControl(MachineControl::ReceiverId);
    MachineServiceGatewayExecutionBundle MachineServiceGateway(MachineServiceGateway::ReceiverId,
                                                               configuration);

    std::array<IComponentExecutionBundle*, 1> bundles{&machineControl};

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
