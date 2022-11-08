///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#include "GpioPin.hpp"
#include "Logger.hpp"

using namespace sugo::hal;

bool GpioPin::init(const IConfiguration& configuration)
{
    m_direction =
        (configuration.getOption("direction").get<std::string>() == "in" ? Direction::In
                                                                         : Direction::Out);
    LOG(debug) << getId() << ".pin: " << configuration.getOption("pin").get<unsigned>();
    LOG(debug) << getId() << ".direction: " << m_direction;
    LOG(debug) << getId()
               << ".activate-high: " << configuration.getOption("active-high").get<bool>();
    return true;
}

IGpioPin::State GpioPin::getState() const
{
    return m_state;
}

bool GpioPin::setState(GpioPin::State state)
{
    m_state = state;
    return false;
}

IGpioPin::Direction GpioPin::getDirection() const
{
    return m_direction;
}

IGpioPin::Event GpioPin::waitForEvent(std::chrono::nanoseconds timeout)
{
    return Event{timeout, EventType::Timeout};
}
