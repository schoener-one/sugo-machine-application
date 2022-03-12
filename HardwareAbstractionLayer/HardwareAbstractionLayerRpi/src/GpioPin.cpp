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
    m_pin        = configuration.getOption("pin").get<unsigned>();
    m_activeHigh = configuration.getOption("active-high").get<bool>();
    m_direction =
        (configuration.getOption("direction").get<std::string>() == "in" ? Direction::In
                                                                         : Direction::Out);
    LOG(debug) << getId() << ".pin: " << m_pin;
    LOG(debug) << getId() << ".direction: " << m_direction;
    LOG(debug) << getId() << ".activate-high: " << m_activeHigh;
    return true;
}

IGpioPin::State GpioPin::getState() const
{
    return State::Low;
}

bool GpioPin::setState(GpioPin::State state)
{
    (void)state;
    return false;
}

IGpioPin::Direction GpioPin::getDirection() const
{
    return Direction::In;
}

bool GpioPin::setDirection(GpioPin::Direction direction)
{
    (void)direction;
    return false;
}

IGpioPin::Event GpioPin::waitForEvent(std::chrono::microseconds timeout)
{
    (void)timeout;
    return Event::TimeoutEvent;
}
