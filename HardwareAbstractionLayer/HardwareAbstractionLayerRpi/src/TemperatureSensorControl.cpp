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
#include "Logger.hpp"
#include "SpiControl.hpp"
#include "TemperatureSensor.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>

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
    assert(m_spiControl == nullptr);

    const std::string device =
        std::string("/dev/") + configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": using SPI device '" << device << "'";

    m_spiControl = std::make_unique<SpiControl>();
    if (!m_spiControl->init(device))
    {
        LOG(error) << getId() << ": Failed to initialize SPI device: " << device;
        finalize();
        return false;
    }

    bool success = initEnabledSubComponents<ITemperatureSensor, TemperatureSensor, SpiControl&,
                                            const IGpioControl::GpioPinMap&>(
        configuration, option::id::TemperatureSensor, m_temperatureSensorMap, *m_spiControl,
        m_gpioPins);

    return success;
}

void TemperatureSensorControl::finalize()
{
    if (m_spiControl != nullptr)
    {
        m_spiControl.reset();
        m_spiControl = nullptr;
    }
    m_temperatureSensorMap.clear();
}
