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

using namespace sugo::hal;

bool AdcInput::init(const IConfiguration& configuration)
{
    m_channel  = configuration.getOption("input").get<unsigned>();
    m_filter = configuration.getOption("filter").get<std::string>();
    LOG(debug) << getId() << ": channel set to " << m_channel;
    LOG(debug) << getId() << ": filter set to " << m_filter;
    return true;
}

IAdcFilter::RawValueType AdcInput::getRawValue() const
{
    return 0u;
}