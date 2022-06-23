///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include "TemperatureSensorControl.hpp"
#include "HalHelper.hpp"
#include "TemperatureSensor.hpp"

namespace sugo::hal
{
class SpiControl
{
public:
    SpiControl() = default;
};
}  // namespace sugo::hal

using namespace sugo::hal;

TemperatureSensorControl::~TemperatureSensorControl()
{
}

bool TemperatureSensorControl::init(const IConfiguration& configuration)
{
    static SpiControl s_spiControlDummy;
    return initEnabledSubComponents<ITemperatureSensor, TemperatureSensor, SpiControl&,
                                    const IGpioControl::GpioPinMap&>(
        configuration, "temperature-sensor", m_temperatureSensorMap, s_spiControlDummy, m_gpioPins);
}

void TemperatureSensorControl::finalize()
{
}