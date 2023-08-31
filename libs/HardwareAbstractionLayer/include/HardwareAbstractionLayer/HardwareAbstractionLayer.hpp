///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common/IConfiguration.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"

#ifdef HAL_SIMULATION_ENABLE
#include <memory>

namespace sugo::hal
{
class Simulator;
}  // namespace sugo::hal
#endif  // HAL_SIMULATION_ENABLE

namespace sugo::hal
{
/// @brief Base class of the hardware abstraction layer
class HardwareAbstractionLayer : public IHardwareAbstractionLayer
{
public:
    HardwareAbstractionLayer();
    ~HardwareAbstractionLayer() override;

    bool init(const common::IConfiguration& configuration) override;
    void finalize();

    const GpioControllerMap& getGpioControllerMap() const override
    {
        return m_gpioControllerMap;
    }

    const StepperMotorControllerMap& getStepperMotorControllerMap() const override
    {
        return m_stepperMotorControllerMap;
    }

    const TemperatureSensorControllerMap& getTemperatureSensorControllerMap() const override
    {
        return m_temperatureSensorControllerMap;
    }

private:
    /// @deprecated To be removed!
    void postInitialization();
    /// @deprecated To be removed!
    void postFinalization();

    GpioControllerMap              m_gpioControllerMap;
    StepperMotorControllerMap      m_stepperMotorControllerMap;
    TemperatureSensorControllerMap m_temperatureSensorControllerMap;
#ifdef HAL_SIMULATION_ENABLE
    std::unique_ptr<Simulator> m_simulator;
#endif  // HAL_SIMULATION_ENABLE
};

}  // namespace sugo::hal
