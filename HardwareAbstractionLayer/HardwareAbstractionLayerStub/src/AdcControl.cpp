///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include "AdcControl.hpp"
#include "AdcFilter.hpp"
#include "AdcInput.hpp"
#include "HalHelper.hpp"

namespace sugo::hal
{
class AdcHat
{
public:
    AdcHat() = default;
};
}  // namespace sugo::hal

using namespace sugo::hal;

AdcControl::~AdcControl()
{
}

bool AdcControl::init(const IConfiguration& configuration)
{
    bool success = initEnabledSubComponents<IAdcFilter, AdcFilter>(configuration, "adc-filter",
                                                                   m_adcFilterMap);

    static AdcHat s_adcHatDummy;

    if (success)
    {
        success = initEnabledSubComponents<IAdcInput, AdcInput, const AdcFilterMap&, AdcHat&>(
            configuration, "adc", m_adcInputMap, m_adcFilterMap, s_adcHatDummy);
    }

    return success;
}

void AdcControl::finalize()
{
}