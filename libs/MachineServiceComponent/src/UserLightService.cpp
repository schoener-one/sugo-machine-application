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

#include "MachineServiceComponent/UserLightService.hpp"

using namespace sugo;
using namespace sugo::machine_service_component;

UserLightService::UserLightService(hal::Identifier powerLightId, hal::Identifier runLightId,
                                   hal::Identifier               readyLightId,
                                   const common::ServiceLocator& serviceLocator)
    : HardwareService(serviceLocator.get<hal::IHardwareAbstractionLayer>()),
      m_powerLightId(std::move(powerLightId)),
      m_runLightId(std::move(runLightId)),
      m_readyLightId(std::move(readyLightId))
{
}

void UserLightService::switchLight(bool powerLightOn, bool runLightOn, bool readyLightOn)
{
    getGpioPin(hal::id::GpioPinRelaySwitchLightPower)
        ->setState(powerLightOn ? hal::IGpioPin::State::High : hal::IGpioPin::State::Low);
    getGpioPin(hal::id::GpioPinRelaySwitchLightRun)
        ->setState(runLightOn ? hal::IGpioPin::State::High : hal::IGpioPin::State::Low);
    getGpioPin(hal::id::GpioPinRelaySwitchLightReady)
        ->setState(readyLightOn ? hal::IGpioPin::State::High : hal::IGpioPin::State::Low);
}
