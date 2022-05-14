///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IAdcFilter.hpp"

namespace sugo::hal
{
/**
 * @brief Class represents an ADC value conversion filter
 *
 */
class AdcFilter : public IAdcFilter
{
public:
    using IAdcFilter::IAdcFilter;

    bool init(const IConfiguration& configuration) override;

    ValueType convert(RawValueType rawValue) const override;

private:
    enum AdcValueTranslation
    {
        Ident,
        Voltage,
        Resistance,
        Ntc100k
    };

    int                 m_refVoltage     = 0;
    unsigned            m_mainResistance = 0;
    unsigned            m_divResistance  = 0;
    AdcValueTranslation m_translation    = AdcValueTranslation::Ident;
};

}  // namespace sugo::hal