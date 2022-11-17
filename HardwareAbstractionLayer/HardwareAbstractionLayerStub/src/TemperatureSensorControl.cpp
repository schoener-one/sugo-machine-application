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
    unsigned m_dummy = 0;
};
}  // namespace sugo::hal

using namespace sugo::hal;

TemperatureSensorControl::TemperatureSensorControl(const Identifier&               id,
                                                   const IGpioControl::GpioPinMap& gpioPins)
    : ITemperatureSensorControl(id), m_gpioPins(gpioPins)
{
}

TemperatureSensorControl::~TemperatureSensorControl()
{
    finalize();
}

bool TemperatureSensorControl::init(const IConfiguration& configuration)
{
    m_spiControl = std::make_unique<SpiControl>();
    return initEnabledSubComponents<ITemperatureSensor, TemperatureSensor, SpiControl&,
                                    const IGpioControl::GpioPinMap&>(
        configuration, option::id::TemperatureSensor, m_temperatureSensorMap, *m_spiControl,
        m_gpioPins);
}

void TemperatureSensorControl::finalize()
{
    m_spiControl.reset();
}