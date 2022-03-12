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
#include "AdcInput.hpp"
#include "HalHelper.hpp"

using namespace sugo::hal;

bool AdcControl::init(const IConfiguration& configuration)
{
    return initEnabledSubComponents<IAdcInput, AdcInput>(configuration, "adc", m_adcInputMap);
}
