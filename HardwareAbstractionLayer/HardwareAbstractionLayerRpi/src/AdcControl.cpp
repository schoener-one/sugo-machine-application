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
#include "AdcHat.hpp"
#include "AdcInput.hpp"
#include "HalHelper.hpp"
#include "Logger.hpp"
#include "SpiControl.hpp"

using namespace sugo::hal;

AdcControl::~AdcControl()
{
    finalize();
}

bool AdcControl::init(const IConfiguration& configuration)
{
    assert(m_spiControl == nullptr);
    assert(m_adcHat == nullptr);

    const std::string device = std::string("/dev/") + configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": using SPI device '" << device << "'";

    m_spiControl = new SpiControl;
    if (!m_spiControl->init(device))
    {
        LOG(error) << getId() << ": Failed to initialize SPI device: " << device;
        finalize();
        return false;
    }

    m_adcHat = new AdcHat(*m_spiControl, m_ioAdcHatCs, m_ioAdcHatRst, m_ioAdcHatRdy);
    if (!m_adcHat->init(AdcHat::DataRate::R1200SPS))
    {
        LOG(error) << getId() << ": Failed to initialize ADC-HAT device";
        finalize();
        return false;
    }

    bool success = initEnabledSubComponents<IAdcFilter, AdcFilter>(configuration, "adc-filter",
                                                                   m_adcFilterMap);

    if (success)
    {
        success = initEnabledSubComponents<IAdcInput, AdcInput, const AdcFilterMap&, AdcHat&>(
            configuration, "adc", m_adcInputMap, m_adcFilterMap, *m_adcHat);
    }

    return success;
}

void AdcControl::finalize()
{
    if (m_adcHat != nullptr)
    {
        delete m_adcHat;
        m_adcHat = nullptr;
    }
    if (m_spiControl != nullptr)
    {
        delete m_spiControl;
        m_spiControl = nullptr;
    }
    m_adcInputMap.clear();
    m_adcFilterMap.clear();
}