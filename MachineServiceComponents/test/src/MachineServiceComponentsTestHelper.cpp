//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineServiceComponentsTestHelper.hpp"
#include "MachineConfig.hpp"

using namespace sugo::test;
using namespace sugo;

using ::testing::ReturnRef;

unsigned MachineConfiguration::MotorSpeedDefault    = 50;
int32_t  MachineConfiguration::DefaultTemperature   = 25;
int      MachineConfiguration::HeaterTemperatureMax = 205;
int      MachineConfiguration::HeaterTemperatureMin = 180;
unsigned MachineConfiguration::MotorSpeedIncrement  = 10u;
unsigned MachineConfiguration::MotorSpeedMax        = 100u;
unsigned MachineConfiguration::ObservationTimeout   = 1000u;

void MachineConfiguration::prepareOptions(IConfigurationMock& mock)
{
    m_optionMotorSpeedDefault    = {id::MotorSpeedDefault, static_cast<unsigned>(MotorSpeedDefault),
                                 ""};
    m_optionMotorSpeedMax        = {id::MotorSpeedMax, static_cast<unsigned>(MotorSpeedMax), ""};
    m_optionMotorSpeedIncrement  = {id::MotorSpeedIncrement,
                                   static_cast<unsigned>(MotorSpeedIncrement), ""};
    m_optionHeaterTemperatureMax = {id::HeaterTemperatureMax,
                                    static_cast<int>(HeaterTemperatureMax), ""};
    m_optionHeaterTemperatureMin = {id::HeaterTemperatureMin,
                                    static_cast<int>(HeaterTemperatureMin), ""};
    m_optionObservationTimeoutGpioPin     = {id::ObservationTimeoutGpioPin,
                                         static_cast<unsigned>(ObservationTimeout), ""};
    m_optionObservationTimeoutTemperature = {id::ObservationTimeoutTemperature,
                                             static_cast<unsigned>(ObservationTimeout), ""};

    ON_CALL(mock, getOption(id::MotorSpeedDefault))
        .WillByDefault(ReturnRef(m_optionMotorSpeedDefault));
    ON_CALL(mock, getOption(id::MotorSpeedMax)).WillByDefault(ReturnRef(m_optionMotorSpeedMax));
    ON_CALL(mock, getOption(id::MotorSpeedIncrement))
        .WillByDefault(ReturnRef(m_optionMotorSpeedIncrement));
    ON_CALL(mock, getOption(id::HeaterTemperatureMax))
        .WillByDefault(ReturnRef(m_optionHeaterTemperatureMax));
    ON_CALL(mock, getOption(id::HeaterTemperatureMin))
        .WillByDefault(ReturnRef(m_optionHeaterTemperatureMin));
    ON_CALL(mock, getOption(id::ObservationTimeoutGpioPin))
        .WillByDefault(ReturnRef(m_optionObservationTimeoutGpioPin));
    ON_CALL(mock, getOption(id::ObservationTimeoutTemperature))
        .WillByDefault(ReturnRef(m_optionObservationTimeoutTemperature));
}
