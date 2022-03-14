///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-12
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Configuration.hpp"
#include "IHalObject.hpp"
#include "Logger.hpp"

namespace sugo::hal
{
template <class ISubCompT, class SubCompT>
inline bool initSubComponents(const IConfiguration& configuration, const std::string& baseName,
                              const std::string&                                            subId,
                              std::map<IHalObject::Identifier, std::unique_ptr<ISubCompT>>& map)
{
    Configuration subConfig;
    configuration.extract(baseName + "." + subId + ".", subConfig);
    auto subComponent = std::make_unique<SubCompT>(subId);

    if (!subComponent->init(subConfig))
    {
        LOG(error) << "failed to initialize: " << subId;
        return false;
    }
    map.insert({subId, std::move(subComponent)});
    return true;
}

template <class ISubCompT, class SubCompT, class SubCompParamT>
inline bool initSubComponents(
    const IConfiguration& configuration, const std::string& baseName, const std::string& subId,
    std::map<IHalObject::Identifier, std::unique_ptr<ISubCompT>>& map, SubCompParamT param)
{
    Configuration subConfig;
    configuration.extract(baseName + "." + subId + ".", subConfig);
    auto subComponent = std::make_unique<SubCompT>(subId, param);

    if (!subComponent->init(subConfig))
    {
        LOG(error) << "failed to initialize: " << subId;
        return false;
    }
    map.insert({subId, std::move(subComponent)});
    return true;
}

template <class ISubCompT, class SubCompT>
inline bool initEnabledSubComponents(
    const IConfiguration& configuration, const std::string& prefixId,
    std::map<IHalObject::Identifier, std::unique_ptr<ISubCompT>>& map)
{
    auto&       option = configuration.getOption(prefixId + "-enabled");
    const auto& subIds = option.get<const std::vector<std::string>&>();
    for (auto& subId : subIds)
    {
        if (!initSubComponents<ISubCompT, SubCompT>(configuration, prefixId, subId, map))
        {
            return false;
        }
    }
    return true;
}

template <class ISubCompT, class SubCompT, class SubCompParamT>
inline bool initEnabledSubComponents(
    const IConfiguration& configuration, const std::string& prefixId,
    std::map<IHalObject::Identifier, std::unique_ptr<ISubCompT>>& map, SubCompParamT param)
{
    auto&       option = configuration.getOption(prefixId + "-enabled");
    const auto& subIds = option.get<const std::vector<std::string>&>();
    for (auto& subId : subIds)
    {
        if (!initSubComponents<ISubCompT, SubCompT, SubCompParamT>(configuration, prefixId, subId, map, param))
        {
            return false;
        }
    }
    return true;
}
}  // namespace sugo::hal
