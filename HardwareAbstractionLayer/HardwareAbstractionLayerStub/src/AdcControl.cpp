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

using namespace sugo::hal;

AdcControl::~AdcControl()
{
}

bool AdcControl::init(const IConfiguration& configuration)
{
    bool success = initEnabledSubComponents<IAdcFilter, AdcFilter>(configuration, "adc-filter",
                                                                   m_adcFilterMap);

    if (success)
    {
        success = initEnabledSubComponents<IAdcInput, AdcInput, const AdcFilterMap&>(
            configuration, "adc", m_adcInputMap, m_adcFilterMap);
    }

    return success;
}

void AdcControl::finalize()
{
}