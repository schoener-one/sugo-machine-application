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

namespace sugo::remote_control
{
namespace def
{
inline static const std::string ConfigNetworkAddress{"localhost"};
inline constexpr unsigned short ConfigPortNumber = 8080u;
inline static const std::string ConfigDocRoot{"www"};
}  // namespace def

namespace id
{
inline static const std::string ConfigRemoteControl{"remote-control"};
inline static const std::string ConfigNetworkAddress{ConfigRemoteControl + ".address"};
inline static const std::string ConfigNetworkAddressDescription{
    "Network address of the remote control"};
inline static const std::string ConfigNetworkPort{ConfigRemoteControl + ".port"};
inline static const std::string ConfigNetworkPortDescription{"Network port of the remote control"};
inline static const std::string ConfigDocRoot{ConfigRemoteControl + ".doc-root"};
inline static const std::string ConfigDocRootDescription{"Web server document root"};
}  // namespace id

namespace config
{
/**
 * @brief Adds remote control configuration options.
 *
 * @param configuration Configuration object.
 */
void addConfigurationOptions(common::IConfiguration& configuration);
}  // namespace config
}  // namespace sugo::remote_control