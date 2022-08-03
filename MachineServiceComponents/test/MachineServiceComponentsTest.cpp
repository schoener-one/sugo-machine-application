//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "FilamentCoilMotor.hpp"
#include "ICommandMessageBrokerMock.hpp"
#include "IHardwareAbstractionLayerMock.hpp"
#include "IStepperMotorControlMock.hpp"
#include "IStepperMotorMock.hpp"
#include "Logger.hpp"
#include "ServiceLocator.hpp"

using namespace sugo;
using namespace hal;

using ::testing::Return;
using ::testing::ReturnRef;

class MachineApplicationSmokeTest : public ::testing::Test
{
protected:
    MachineApplicationSmokeTest()
    {
    }

    ~MachineApplicationSmokeTest() override
    {
    }

    void SetUp() override
    {
        auto stepperMotorControl = std::make_unique<IStepperMotorControlMock>();
        ON_CALL(m_mockHardwareAbstractionLayer, getStepperMotorControllerMap())
            .WillByDefault(ReturnRef(m_stepperMotorControllerMap));
        ON_CALL(*stepperMotorControl, getStepperMotorMap())
            .WillByDefault(ReturnRef(m_stepperMotorMap));
        m_stepperMotorControllerMap.emplace(StepperMotorControlName, stepperMotorControl);
        m_stepperMotorMap.emplace(StepperMotorName, std::make_unique<IStepperMotorMock>());
        m_serviceLocator.add<IHardwareAbstractionLayer>(m_mockHardwareAbstractionLayer);
    }

    void TearDown() override
    {
    }

    inline static const std::string StepperMotorName{"coiler"};
    inline static const std::string StepperMotorControlName{"stepper-motor-control"};

    IHardwareAbstractionLayer::StepperMotorControllerMap m_stepperMotorControllerMap;
    IStepperMotorControl::StepperMotorMap                m_stepperMotorMap;

    IHardwareAbstractionLayerMock m_mockHardwareAbstractionLayer;
    ICommandMessageBrokerMock     m_mockCommandMessageBroker;
    ServiceLocator                m_serviceLocator;
};

TEST_F(MachineApplicationSmokeTest, FilamentCoilMotor_Start)
{
    FilamentCoilMotor comp(m_mockCommandMessageBroker, m_serviceLocator);
}
