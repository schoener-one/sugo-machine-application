
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-28
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IStepperMotor.hpp"
#include "TicController.hpp"

namespace sugo::hal
{
/**
 * @brief Control class for the Tic T249 stepper motor controller.
 *
 */
class TicControlStepperMotor : public IStepperMotor, protected TicController
{
public:
    TicControlStepperMotor(const Identifier& id, Address address, I2cControl& i2cControl,
                           IGpioPin& ioErr, IGpioPin& ioRst)
        : IStepperMotor(id), TicController(address, i2cControl, ioErr, ioRst)
    {
    }
    ~TicControlStepperMotor();

    bool init(const IConfiguration& configuration) override;
    void finalize();

    unsigned rotate(unsigned steps, Direction direction, unsigned speed) override;
    bool     stop() override;
    unsigned getMicroStepCount() const override;
    unsigned getStepsPerRound() const override;
    unsigned getSpeed() const override;
    unsigned getMaxSpeed() const override;
};

}  // namespace sugo::hal