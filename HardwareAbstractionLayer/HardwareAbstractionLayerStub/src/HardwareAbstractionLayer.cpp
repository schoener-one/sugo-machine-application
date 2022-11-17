///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-11-22
 */
///////////////////////////////////////////////////////////////////////////////

#include "HardwareAbstractionLayer.hpp"
#include "Simulator.hpp"

using namespace sugo::hal;

HardwareAbstractionLayer::HardwareAbstractionLayer()
    : IHardwareAbstractionLayer(id::HardwareAbstractionLayer)
{
}

HardwareAbstractionLayer::~HardwareAbstractionLayer()  // NOLINT(modernize-use-equals-default)
{
}

void HardwareAbstractionLayer::postInitialization()
{
    Simulator&  simulator      = Simulator::getInstance();
    const auto& gpioController = m_gpioControllerMap.at(id::GpioControl);

    for (auto& [key, pin] : gpioController->getGpioPinMap())
    {
        simulator.registerGpioPin(key, pin->getDirection());
    }

    const auto& temperatureSensorControl =
        m_temperatureSensorControllerMap.at(id::TemperatureSensorControl);

    for (auto& [key, tempSensor] : temperatureSensorControl->getTemperatureSensorMap())
    {
        simulator.registerTemperatureSensor(key);
    }
}

void HardwareAbstractionLayer::postFinalization()
{
    Simulator&  simulator      = Simulator::getInstance();
    const auto& gpioController = m_gpioControllerMap.at(id::GpioControl);

    for (auto& [key, pin] : gpioController->getGpioPinMap())
    {
        simulator.unregisterGpioPin(key);
    }

    const auto& temperatureSensorControl =
        m_temperatureSensorControllerMap.at(id::TemperatureSensorControl);

    for (auto& [key, tempSensor] : temperatureSensorControl->getTemperatureSensorMap())
    {
        simulator.unregisterTemperatureSensor(key);
    }
}