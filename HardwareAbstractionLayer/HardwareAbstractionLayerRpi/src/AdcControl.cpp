///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include <wsadhat/ADS1263.h>
#include <wsadhat/DEV_Config.h>
}

#include "AdcControl.hpp"
#include "AdcFilter.hpp"
#include "AdcInput.hpp"
#include "HalHelper.hpp"
#include "Logger.hpp"

using namespace sugo::hal;

AdcControl::~AdcControl()
{
    finalize();
}

bool AdcControl::init(const IConfiguration& configuration)
{
    m_adcInputMap.clear();
    m_adcFilterMap.clear();

    if (DEV_Module_Init() != 0)
    {
        LOG(error) << getId() << ": failed to initialize ADC";
        return false;
    }

    ADS1263_SetMode(0u);

    if (ADS1263_init_ADC1(ADS1263_1200SPS) != 0)
    {
        LOG(error) << getId() << ": failed to initialize ADC1";
        finalize();
        return false;
    }

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
    m_adcInputMap.clear();
    DEV_Module_Exit();
}