///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "GpioPinEventObserver.hpp"

#include <cassert>

using namespace sugo;
using namespace sugo::hal;

GpioPinEventObserver::GpioPinEventObserver(
    const std::shared_ptr<IGpioPin> pin, EventHandler eventHandler,
    const std::chrono::milliseconds& maxGpioPinObservationTimeout)
    : m_pin(pin),
      m_eventHandler(eventHandler),
      m_maxGpioPinObservationTimeout(maxGpioPinObservationTimeout),
      m_thread(m_pin->getId() + "Observer")
{
}

GpioPinEventObserver::~GpioPinEventObserver()
{
    if (isRunning())
    {
        stop();
    }
}

bool GpioPinEventObserver::start()
{
    assert(!m_thread.isRunning());
    assert(m_pin);
    assert(m_eventHandler);
    m_doObserve = true;

    return m_thread.start([this] {
        while (m_doObserve)
        {
            const auto event =
                this->m_pin->waitForEvent(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    this->m_maxGpioPinObservationTimeout));

            if (m_doObserve && (event.type != IGpioPin::EventType::Timeout))
            {
                this->m_eventHandler(event, this->getId());
            }
        }
    });
}

void GpioPinEventObserver::stop()
{
    if (isRunning())
    {
        m_doObserve = false;
        m_thread.join();
        m_pin.reset();
    }
}
