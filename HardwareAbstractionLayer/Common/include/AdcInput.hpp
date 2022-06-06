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

#include <limits>

namespace sugo::hal
{
class AdcHat;

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

    AdcInput(const Identifier& id, const AdcFilterMap& filterProvider, AdcHat& adcHat)
        : IAdcInput(id), m_filterProvider(filterProvider), m_adcHat(adcHat)
    {
    }

    bool init(const IConfiguration& configuration) override;

    IAdcFilter::RawValueType getRawValue() const override;

    IAdcFilter::ValueType getUnitValue() const override
    {
        return m_filterProvider.at(m_filter)->convert(getRawValue());
    }

    void setCorrectionValues(IAdcFilter::RawValueType valueOffset, IAdcFilter::RawValueType valueMax)
    {
        m_valueOffset = valueOffset;
        m_valueMax = valueMax;
    }

private:
    const AdcFilterMap&      m_filterProvider;
    AdcHat&                  m_adcHat;
    std::string              m_filter;
    unsigned                 m_channel     = InvalidChannel;
    IAdcFilter::RawValueType m_valueOffset = 0u;
    IAdcFilter::RawValueType m_valueMax    = std::numeric_limits<IAdcFilter::RawValueType>::max();
};

}  // namespace sugo::hal
