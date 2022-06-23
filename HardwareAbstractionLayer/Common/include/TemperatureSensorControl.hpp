///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioControl.hpp"
#include "ITemperatureSensorControl.hpp"

namespace sugo::hal
{
class SpiControl;

/**
 * @brief Class for contolling ADC input devices
 *
 */
class TemperatureSensorControl : public ITemperatureSensorControl
{
public:
    TemperatureSensorControl(const Identifier& id, const IGpioControl::GpioPinMap& gpioPins)
        : ITemperatureSensorControl(id), m_gpioPins(gpioPins)
    {
    }
    ~TemperatureSensorControl() override;

    bool init(const IConfiguration& configuration) override;

    void finalize();

    const TemperatureSensorMap& getTemperatureSensorMap() override
    {
        return m_temperatureSensorMap;
    }

private:
    const IGpioControl::GpioPinMap& m_gpioPins;
    TemperatureSensorMap            m_temperatureSensorMap;
    SpiControl*                     m_spiControl = nullptr;
};

}  // namespace sugo::hal
