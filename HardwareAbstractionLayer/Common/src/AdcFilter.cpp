///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-16
 */
///////////////////////////////////////////////////////////////////////////////

#include "AdcFilter.hpp"
#include "Logger.hpp"

namespace
{
constexpr unsigned s_maxNumberOfChannels = 10u;
}

using namespace sugo::hal;

bool AdcFilter::init(const IConfiguration& configuration)
{
    m_refVoltage = configuration.getOption("ref-voltage").get<int>();
    m_gain = configuration.getOption("gain").get<unsigned>();
    m_mainResistance = configuration.getOption("resistance").get<unsigned>();
    m_divResistance = configuration.getOption("divider-resistance").get<unsigned>();
    std::string unitName = configuration.getOption("unit").get<std::string>();
    LOG(debug) << getId() << ": ref-voltage set to " << m_refVoltage;
    LOG(debug) << getId() << ": gain set to " << m_gain;
    LOG(debug) << getId() << ": main resistance set to " << m_mainResistance;
    LOG(debug) << getId() << ": divider resistance set to " << m_divResistance;
    LOG(debug) << getId() << ": unit name set to " << unitName;

    return true;
}
