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

#include <cassert>

#include "Common/Configuration.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/HalTypes.hpp"
#include "HardwareAbstractionLayer/IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Initializes sub components according to configuration.
 *
 * @tparam ISubCompT
 * @tparam SubCompT
 * @tparam SubCompParams
 * @param configuration
 * @param baseName
 * @param subId
 * @param map
 * @param params
 * @return true
 * @return false
 */
template <class ISubCompT, class SubCompT, class... SubCompParams>
inline bool initSubComponents(const common::IConfiguration& configuration,
                              const std::string& baseName, const std::string& subId,
                              std::map<Identifier, std::shared_ptr<ISubCompT>>& map,
                              SubCompParams&... params)
{
    common::Configuration subConfig;
    configuration.extract(baseName + "." + subId + ".", subConfig);
    auto subComponent = std::make_shared<SubCompT>(subId, params...);

    if (!subComponent->init(subConfig))
    {
        LOG(error) << "failed to initialize: " << subId;
        return false;
    }
    map.insert({subId, std::move(subComponent)});
    return true;
}

/**
 * @brief Initializes enabled sub components according to configuration.
 *
 * @tparam ISubCompT
 * @tparam SubCompT
 * @tparam SubCompParams
 * @param configuration
 * @param prefixId
 * @param map
 * @param params
 * @return true
 * @return false
 */
template <class ISubCompT, class SubCompT, class... SubCompParams>
inline bool initEnabledSubComponents(const common::IConfiguration& configuration,
                                     const std::string&            prefixId,
                                     std::map<Identifier, std::shared_ptr<ISubCompT>>& map,
                                     SubCompParams&... params)
{
    auto&       option = configuration.getOption(prefixId + "-enabled");
    const auto& subIds = option.get<const std::vector<std::string>&>();
    for (auto& subId : subIds)
    {
        if (!initSubComponents<ISubCompT, SubCompT, SubCompParams...>(configuration, prefixId,
                                                                      subId, map, params...))
        {
            return false;
        }
    }
    return true;
}
}  // namespace sugo::hal
