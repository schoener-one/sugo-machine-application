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

#include <string>

namespace sugo::machine_service_component::id
{
inline static const std::string Result{"result"};
inline static const std::string Speed{"speed"};
inline static const std::string TensionControl{"tension-control"};
inline static const std::string Type{"type"};
inline static const std::string Temperature{"temperature"};
inline static const std::string ErrorSetMotorSpeedOutOfRange{"error-setmotorspeed-outofrange"};
inline static const std::string ErrorParameterInvalid{"error-parameter-invalid"};
}  // namespace sugo::machine_service_component::id