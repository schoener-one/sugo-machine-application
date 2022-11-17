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
#include "Simulator.hpp"

using namespace sugo::hal;

GpioPin::~GpioPin()
{
    finalize();
}

bool GpioPin::init(const IConfiguration& configuration)
{
    m_direction =
        (configuration.getOption("direction").get<std::string>() == "in" ? Direction::In
                                                                         : Direction::Out);
    LOG(debug) << getId() << ".pin: " << configuration.getOption("pin").get<unsigned>();
    LOG(debug) << getId() << ".direction: " << m_direction;
    LOG(debug) << getId()
               << ".active-high: " << configuration.getOption(option::id::ActiveHigh).get<bool>();

    return true;
}

void GpioPin::finalize()
{
}

IGpioPin::State GpioPin::getState() const
{
    return Simulator::getInstance().getState(getId());
}

bool GpioPin::setState(GpioPin::State state)
{
    return Simulator::getInstance().setState(getId(), state);
}

IGpioPin::Direction GpioPin::getDirection() const
{
    return m_direction;
}

IGpioPin::Event GpioPin::waitForEvent(std::chrono::nanoseconds timeout)
{
    return Event{timeout, EventType::Timeout};
}
