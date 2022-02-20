///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-02-20
 */
///////////////////////////////////////////////////////////////////////////////

#include "IHardwareAbstractionLayer.hpp"
#include "HardwareAbstractionLayerImpl.hpp"

namespace sugo
{
IHardwareAbstractionLayer& IHardwareAbstractionLayer::get()
{
    static HardwareAbstractionLayerImpl halStub{};
    return halStub;
}

void IHardwareAbstractionLayer::setConfiguration(IConfiguration& configuration)
{
    configuration.add(Option("hardware-abstraction-layer.gpio.logic-active-high", false,
                             "HAL: Indicates if GPIO logic is active-high or -low"));
}
}  // namespace sugo