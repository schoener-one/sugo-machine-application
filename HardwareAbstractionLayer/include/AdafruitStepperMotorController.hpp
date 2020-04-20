/*
 * AdafruitStepperMotorController.hpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */

#ifndef ADAFRUITSTEPPERMOTORCONTROLLER_HPP_
#define ADAFRUITSTEPPERMOTORCONTROLLER_HPP_

#include <IStepperMotorController.hpp>
#include <memory>

namespace moco
{
/**
 * Class implements motor controlling for Adafruit motor HAT controller.
 */
class AdafruitStepperMotorController : public IStepperMotorController
{
public:
    constexpr static unsigned StepperMotorCount = 2u;

    AdafruitStepperMotorController();
    ~AdafruitStepperMotorController();

    // IStepperMotorController {{
    void                  reset() override;
    unsigned              getStepperMotorCount() const override { return StepperMotorCount; }
    IStepperMotorControl& getStepperMotorControl(unsigned number) override;
    // IStepperMotorController }}

private:
    class MotorHAT;
    std::unique_ptr<MotorHAT> m_motorHat;
};

}  // namespace moco

#endif /* ADAFRUITSTEPPERMOTORCONTROLLER_HPP_ */
