///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioPin.hpp"
#include "IStepperMotor.hpp"

namespace sugo::hal
{
class TicController;
class I2cControl;

/**
 * @brief Class represents an ADC input
 *
 */
class StepperMotor : public IStepperMotor
{
public:
    StepperMotor(const Identifier& id, I2cControl& i2cControl, IGpioPin& ioErr, IGpioPin& ioRst)
        : IStepperMotor(id), m_i2cControl(i2cControl), m_ioErr(ioErr), m_ioRst(ioRst)
    {
    }
    ~StepperMotor();

    bool init(const IConfiguration& configuration) override;
    void finalize();

    bool      reset() override;
    bool      rotateToPosition(Position position) override;
    bool      rotate(Direction direction) override;
    bool      stop(bool stopImmediately = false) override;
    Position  getPosition() const override;
    StepCount getMicroStepCount() const override;
    StepCount getStepsPerRound() const override;
    Speed     getSpeed() const override;
    Speed     getMaxSpeed() const override
    {
        return m_maxSpeed;
    }
    void setMaxSpeed(Speed maxSpeed)
    {
        m_maxSpeed = maxSpeed;
    }

private:
    bool prepareForMotion();
    bool waitAndShutdown(std::chrono::milliseconds remainingMotionTime,
                         bool                      stopImmediately = false);

    I2cControl&    m_i2cControl;
    IGpioPin&      m_ioErr;
    IGpioPin&      m_ioRst;
    TicController* m_controller   = nullptr;
    Speed          m_maxSpeed     = Speed(0, Unit::Rpm);
    Speed          m_initMaxSpeed = Speed(0, Unit::Rpm);
};

}  // namespace sugo::hal
