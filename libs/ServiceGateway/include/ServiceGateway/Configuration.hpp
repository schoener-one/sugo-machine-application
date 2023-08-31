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

namespace sugo::service_gateway
{
namespace def
{
inline static const std::string ConfigNetworkAddress{"*"};
inline constexpr unsigned short ConfigPortNumber = 4242u;
}  // namespace def

namespace id
{
inline static const std::string ConfigServiceGateway{"service-gateway"};
inline static const std::string ConfigNetworkAddress{ConfigServiceGateway + ".address"};
inline static const std::string ConfigNetworkAddressDescription{"Network address of the service"};
inline static const std::string ConfigNetworkPort{ConfigServiceGateway + ".port"};
inline static const std::string ConfigNetworkPortDescription{"Network port of the service"};

}  // namespace id

namespace config
{
/**
 * @brief Adds service gateway configuration options.
 *
 * @param configuration Configuration object.
 */
void addConfigurationOptions(common::IConfiguration& configuration);
}  // namespace config
}  // namespace sugo::service_gateway