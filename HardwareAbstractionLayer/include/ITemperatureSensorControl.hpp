///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-02-24
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <memory>
#include <string>

#include "IHalObject.hpp"
#include "ITemperatureSensor.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for ADC input devices.
 *
 */
class ITemperatureSensorControl : public IHalObject
{
public:
    /// ADC input map
    using TemperatureSensorMap = IHalObject::Map<ITemperatureSensor>;

    /**
     * @brief Destroy the ITemperatureSensorControl object.
     *
     */
    virtual ~ITemperatureSensorControl()
    {
    }

    /**
     * @brief Returns a map of available ADC inputs
     *
     * @return const TemperatureSensorMap& Map of available ADC inputs
     */
    virtual const TemperatureSensorMap& getTemperatureSensorMap() = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
