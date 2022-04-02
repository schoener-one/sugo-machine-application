///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include <wsadhat/ADS1263.h>
}

#include "AdcInput.hpp"
#include "Logger.hpp"

namespace
{
constexpr unsigned s_maxNumberOfChannels = 10u;
}

using namespace sugo::hal;

bool AdcInput::init(const IConfiguration& configuration)
{
    m_channel = configuration.getOption("channel").get<unsigned>();
    m_filter  = configuration.getOption("adc-filter").get<std::string>();
    LOG(debug) << getId() << ": m_channel set to " << m_channel;
    LOG(debug) << getId() << ": filter set to " << m_filter;

    if (m_channel >= s_maxNumberOfChannels)
    {
        LOG(error) << getId() << ": invalid m_channel number " << m_channel;
        return false;
    }
    return true;
}

IAdcFilter::RawValueType AdcInput::getRawValue() const
{
    return ADS1263_GetChannalValue(m_channel);
}
