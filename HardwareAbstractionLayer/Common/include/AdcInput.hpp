///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IAdcFilter.hpp"
#include "IAdcInput.hpp"

namespace wsadhat
{
class AdcHat;
}  // namespace wsadhat

namespace sugo::hal
{
/**
 * @brief Type which represents a list of filters
 *
 */
using AdcFilterMap = IHalObject::Map<IAdcFilter>;

/**
 * @brief Class represents an ADC channel
 *
 */
class AdcInput : public IAdcInput
{
public:
    constexpr static unsigned InvalidChannel = 0xffffffffu;

    AdcInput(const Identifier& id, const AdcFilterMap& filterProvider, wsadhat::AdcHat& adcHat)
        : IAdcInput(id), m_filterProvider(filterProvider), m_adcHat(adcHat)
    {
    }

    bool init(const IConfiguration& configuration) override;

    IAdcFilter::RawValueType getRawValue() const override;

    IAdcFilter::ValueType getValue() const override
    {
        return m_filterProvider.at(m_filter)->convert(getRawValue());
    }

private:
    const AdcFilterMap& m_filterProvider;
    wsadhat::AdcHat&    m_adcHat;
    std::string         m_filter;
    unsigned            m_channel = InvalidChannel;
};

}  // namespace sugo::hal
