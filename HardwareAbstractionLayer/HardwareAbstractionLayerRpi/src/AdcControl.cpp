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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>

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

    calibrate();
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

void AdcControl::calibrate()
{
    const auto input5v = m_adcInputMap.find("reference-5v");
    const auto input2_5v = m_adcInputMap.find("reference-2_5v");
    const auto input0v = m_adcInputMap.find("reference-0v");
    IAdcFilter::RawValueType min = 0;
    IAdcFilter::RawValueType max = std::numeric_limits<IAdcFilter::RawValueType>::max();
    IAdcFilter::RawValueType middle = max / 2;
    if ((input0v != m_adcInputMap.end()) && (input2_5v != m_adcInputMap.end()) && (input5v != m_adcInputMap.end()))
    {
        min = input0v->second->getRawValue();
        max = input5v->second->getRawValue();
        middle = input2_5v->second->getRawValue();
    }
    LOG(debug) << getId() << "ADC range: " << min << "/" << middle << "/" << max;
    for (auto& item : m_adcInputMap)
    {
        if (!boost::starts_with(item.first, "reference-"))
        {
            AdcInput* adcInput = static_cast<AdcInput*>(item.second.get());
            adcInput->setCorrectionValues(min, max);
        }
    }
}