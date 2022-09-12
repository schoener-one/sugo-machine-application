///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioControl.hpp"
#include "ITemperatureSensor.hpp"

#include <limits>

namespace sugo::hal
{
class SpiControl;
class Max31865;

/**
 * @brief Class represents an ADC channel
 *
 */
class TemperatureSensor : public ITemperatureSensor
{
public:
    TemperatureSensor(const Identifier& id, SpiControl& spiControl,
                      const IGpioControl::GpioPinMap& gpioPins)
        : ITemperatureSensor(id), m_spiControl(spiControl), m_gpioPins(gpioPins)
    {
    }
    ~TemperatureSensor() override;

    void finalize();
    bool init(const IConfiguration& configuration) override;

    Temperature getTemperature() const override;

private:
    SpiControl&                     m_spiControl;
    const IGpioControl::GpioPinMap& m_gpioPins;
    Max31865*                       m_driver = nullptr;
};

}  // namespace sugo::hal
