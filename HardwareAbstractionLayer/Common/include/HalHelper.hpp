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
#include "HalTypes.hpp"
#include "IHalObject.hpp"
#include "Logger.hpp"

#include <cassert>

namespace sugo::hal
{
template <class ISubCompT, class SubCompT, class... SubCompParams>
inline bool initSubComponents(const IConfiguration& configuration, const std::string& baseName,
                              const std::string&                                            subId,
                              std::map<IHalObject::Identifier, std::shared_ptr<ISubCompT>>& map,
                              SubCompParams&... params)
{
    Configuration subConfig;
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

template <class ISubCompT, class SubCompT, class... SubCompParams>
inline bool initEnabledSubComponents(
    const IConfiguration& configuration, const std::string& prefixId,
    std::map<IHalObject::Identifier, std::shared_ptr<ISubCompT>>& map, SubCompParams&... params)
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

template <typename ValueT = int32_t>
inline ValueT toInteger(const ByteBuffer& buffer)
{
    assert(buffer.size() >= sizeof(ValueT));
    return static_cast<ValueT>(buffer.at(0)) | (static_cast<ValueT>(buffer.at(1)) << 8) |
           (static_cast<ValueT>(buffer.at(2)) << 16) | (static_cast<ValueT>(buffer.at(3)) << 24);
}
}  // namespace sugo::hal
