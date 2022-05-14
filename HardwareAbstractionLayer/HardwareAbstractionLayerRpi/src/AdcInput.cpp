///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#include "AdcInput.hpp"
#include "Logger.hpp"
#include "AdcHat.hpp"

using namespace sugo::hal;

bool AdcInput::init(const IConfiguration& configuration)
{
    m_channel = configuration.getOption("channel").get<unsigned>();
    m_filter  = configuration.getOption("adc-filter").get<std::string>();
    LOG(debug) << getId() << ": m_channel set to " << m_channel;
    LOG(debug) << getId() << ": filter set to " << m_filter;

    if (m_channel >= AdcHat::MaxChannels)
    {
        LOG(error) << getId() << ": invalid m_channel number " << m_channel;
        return false;
    }
    return true;
}

IAdcFilter::RawValueType AdcInput::getRawValue() const
{
    return static_cast<IAdcFilter::RawValueType>(m_adcHat.getChannelValue(m_channel));
}
