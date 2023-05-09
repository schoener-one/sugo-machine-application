//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "IConfigurationMock.hpp"

namespace sugo::test
{
class MachineConfiguration
{
public:
    static unsigned MotorSpeedDefault;
    static int32_t  DefaultTemperature;
    static int      HeaterTemperatureMax;
    static int      HeaterTemperatureMin;
    static unsigned MotorSpeedIncrement;
    static unsigned MotorSpeedMax;
    static unsigned ObservationTimeout;

    void prepareOptions(IConfigurationMock& mock);

private:
    Option m_optionMotorSpeedDefault{};
    Option m_optionMotorSpeedMax{};
    Option m_optionMotorSpeedIncrement{};
    Option m_optionHeaterTemperatureMax{};
    Option m_optionHeaterTemperatureMin{};
    Option m_optionObservationTimeoutGpioPin{};
    Option m_optionObservationTimeoutTemperature{};
};
}  // namespace sugo::test