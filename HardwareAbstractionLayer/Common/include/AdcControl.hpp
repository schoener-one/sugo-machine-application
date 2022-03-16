///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AdcInput.hpp"
#include "IAdcControl.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling ADC input devices
 *
 */
class AdcControl : public IAdcControl
{
public:
    using IAdcControl::IAdcControl;

    ~AdcControl() override;

    bool init(const IConfiguration& configuration) override;

    void finalize();

    const AdcInputMap& getAdcInputMap() override
    {
        return m_adcInputMap;
    }

private:
    AdcInputMap  m_adcInputMap;
    AdcFilterMap m_adcFilterMap;
};

}  // namespace sugo::hal
