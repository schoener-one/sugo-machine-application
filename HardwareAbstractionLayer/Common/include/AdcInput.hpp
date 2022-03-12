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

#include "IAdcInput.hpp"

namespace sugo::hal
{
/**
 * @brief Class represents an ADC input
 *
 */
class AdcInput : public IAdcInput
{
public:
    using IAdcInput::IAdcInput;

    bool init(const IConfiguration& configuration) override;

    AdcValueType getRawValue() const override;

private:
    unsigned    m_input = InvalidInput;
    std::string m_filter;
};

}  // namespace sugo::hal
