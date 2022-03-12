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
    m_input  = configuration.getOption("input").get<unsigned>();
    m_filter = configuration.getOption("filter").get<std::string>();
    LOG(debug) << getId() << ".input: " << m_input;
    LOG(debug) << getId() << ".filter: " << m_filter;
    return true;
}

IAdcInput::AdcValueType AdcInput::getRawValue() const
{
    return 0;
}