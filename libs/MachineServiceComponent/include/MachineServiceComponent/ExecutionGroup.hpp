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

#include "Common/ServiceLocator.hpp"
#include "MachineServiceComponent/FilamentCoilControl.hpp"
#include "MachineServiceComponent/FilamentCoilMotor.hpp"
#include "MachineServiceComponent/FilamentFeederMotor.hpp"
#include "MachineServiceComponent/FilamentMergerControl.hpp"
#include "MachineServiceComponent/FilamentMergerHeater.hpp"
#include "MachineServiceComponent/FilamentPreHeater.hpp"
#include "MachineServiceComponent/FilamentTensionSensor.hpp"
#include "MachineServiceComponent/MachineControl.hpp"
#include "MachineServiceComponent/UserInterfaceControl.hpp"
#include "ServiceComponent/ExecutionBundle.hpp"
#include "ServiceComponent/ExecutionGroup.hpp"

// TODO Generate this file automatically!
namespace sugo::machine_service_component
{
using MachineControlBundle =
    service_component::ExecutionBundle<MachineControl, const common::ServiceLocator&>;
using FilamentMergerControlBundle =
    service_component::ExecutionBundle<FilamentMergerControl, const common::ServiceLocator&>;
using FilamentFeederMotorBundle =
    service_component::ExecutionBundle<FilamentFeederMotor, const common::ServiceLocator&>;
using FilamentMergerHeaterBundle =
    service_component::ExecutionBundle<FilamentMergerHeater, const common::ServiceLocator&>;
using FilamentPreHeaterBundle =
    service_component::ExecutionBundle<FilamentPreHeater, const common::ServiceLocator&>;
using FilamentCoilControlBundle =
    service_component::ExecutionBundle<FilamentCoilControl, const common::ServiceLocator&>;
using FilamentCoilMotorBundle =
    service_component::ExecutionBundle<FilamentCoilMotor, const common::ServiceLocator&>;
using FilamentTensionSensorBundle =
    service_component::ExecutionBundle<FilamentTensionSensor, const common::ServiceLocator&>;
using UserInterfaceControlBundle =
    service_component::ExecutionBundle<UserInterfaceControl, const common::ServiceLocator&>;

/// @brief Type definition of machine service execution group.
using ExecutionGroup = service_component::ExecutionGroup<
    MachineControlBundle, FilamentMergerControlBundle, FilamentFeederMotorBundle,
    FilamentMergerHeaterBundle, FilamentPreHeaterBundle, FilamentCoilControlBundle,
    FilamentCoilMotorBundle, FilamentTensionSensorBundle, UserInterfaceControlBundle>;
}  // namespace sugo::machine_service_component