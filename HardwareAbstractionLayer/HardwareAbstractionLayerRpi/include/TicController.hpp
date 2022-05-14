
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

#include "HalTypes.hpp"
#include "I2cControl.hpp"
#include "IGpioPin.hpp"
#include "IStepperMotor.hpp"

namespace sugo::hal
{
/**
 * @brief Control class for the Tic T249 stepper motor controller.
 *
 */
class TicController
{
public:
    TicController(I2cControl::Address address, I2cControl& i2cControl, IGpioPin& ioErr,
                  IGpioPin& ioRst);
    ~TicController();

    bool init();
    void finalize();

    /// Stepper mode
    enum StepMode
    {
        Full    = 0,  ///< Full step mode
        Step1_2 = 1,  ///< 1/2 step mode
        Step1_4 = 2,  ///< 1/4 step mode
        Step1_8 = 3   ///< 1/8 step mode
    };

    /// Variable offsets
    enum VariableOffset
    {
        OperationState  = 0x00,  ///< Operation state (unsigned 8-bit)
        MiscFlags       = 0x01,  ///< Misc flags (unsigned 8-bit)
        ErrorStatus     = 0x02,  ///< Error status (unsigned 16-bit)
        ErrorsOccurred  = 0x04,  ///< Error occurred (unsigned 32-bit)
        CurrentPosition = 0x22,  ///< Current position (signed 32-bit)
        CurrentVelocity = 0x26,  ///< Current velocity (signed 32-bit)
        InputState      = 0x4C,  ///< Input state (unsigned 8-bit)
    };

    /**
     * @brief This command sets the target position of the Tic, in microsteps.
     * The Tic will start moving the motor to reach the target position.
     *
     * @param position Position to set.
     *   Range: −2,147,483,648 to +2,147,483,647 = −0x8000 0000 to +0x7FFF FFFF
     *   Units: microsteps
     * @return true if succeeded
     * @return false if failed
     */
    bool setTargetPosition(int32_t position);

    /**
     * @brief This command sets the target velocity of the Tic, in microsteps per 10,000 seconds.
     * The Tic will start accelerating or decelerating the motor to reach the target velocity.
     *
     * @param velocity Velocity to set.
     *   Range: −500,000,000 to +500,000,000.
     *   Units: microsteps per 10,000 s.
     * @return true if succeeded
     * @return false if failed
     */
    bool setTargetVelocity(int32_t velocity);

    /**
     * @brief This command stops the motor abruptly without respecting the deceleration limit and
     * sets the “Current position” variable, which represents what position the Tic currently thinks
     * the motor is in. Besides stopping the motor and setting the current position, this command
     * also clears the “position uncertain” flag, sets the input state to “halt”, and clears the
     * “input after scaling” variable.
     *
     * @param position Current position
     *   Range: −2,147,483,648 to +2,147,483,647 = −0x8000 0000 to +0x7FFF FFFF
     *   Units: microsteps
     * @return true if succeeded
     * @return false if failed
     */
    bool haltAndSetPosition(int32_t position);

    /**
     * @brief This command stops the motor abruptly without respecting the deceleration limit.
     * Besides stopping the motor, this command also sets the “position uncertain” flag (because the
     * abrupt stop might cause steps to be missed), sets the input state to “halt”, and clears the
     * “input after scaling” variable.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool haltAndHold();

    /**
     * @brief Starts the Tic’s homing procedure
     *
     * @param direction Direction to run until homing position is reached.
     * @return true if succeeded
     * @return false if failed
     */
    bool goHome(IStepperMotor::Direction direction = IStepperMotor::Direction::Backward);

    /**
     * @brief If the command timeout is enabled, this command resets it and prevents the “command
     * timeout” error from happening for some time.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool resetCommandTimeout();

    /**
     * @brief This command causes the Tic to de-energize the stepper motor coils by disabling its
     * stepper motor driver. The motor will stop moving and consuming power. This command sets the
     * “position uncertain” flag (because the Tic is no longer in control of the motor’s position);
     * the Tic will also set the “intentionally de-energized” error bit, turn on its red LED, and
     * drive its ERR line high.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool deEnergize();

    /**
     * @brief This command is a request for the Tic to energize the stepper motor coils by enabling
     * its stepper motor driver. The Tic will clear the “intentionally de-energized” error bit. If
     * there are no other errors, this allows the system to start up.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool energize();

    /**
     * @brief In control mode, this command causes the “safe start violation” error to be cleared
     * for 200 ms. If there are no other errors, this allows the system to start up.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool exitSafeStart();

    /**
     * @brief If safe start is enabled and the control mode is RC speed, analog speed, or encoder
     * speed, this command causes the Tic to stop the motor (using the configured soft error
     * response behavior) and set its “safe start violation” error bit. If safe start is disabled,
     * or if the Tic is not in one of the listed modes, this command will cause a brief interruption
     * in motor control (during which the soft error response behavior will be triggered) but
     * otherwise have no effect.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool enterSafeStart();

    /**
     * @brief This command makes the Tic forget most parts of its current state.
     * Specifically, it does the following:
     * - Reloads all settings from the Tic’s non-volatile memory and discards any temporary changes
     *   to the settings previously made with serial commands (this applies to the step mode,
     *   current limit, decay mode, max speed, starting speed, max acceleration, and max
     * deceleration settings).
     * - Abruptly halts the motor • Resets the motor driver • Sets the Tic’s operation state to
     * “reset”.
     * - Clears the last movement command and the current position.
     * - Clears the encoder position.
     * - Clears the serial and “command timeout” errors and the “errors occurred” bits.
     * - Enters safe start if configured to do so.
     *
     * @return true if succeeded
     * @return false if failed
     */
    bool reset();

    /**
     * @brief This command temporarily sets the Tic’s maximum allowed motor speed in units of
     * steps per 10,000 seconds. The provided value will override the corresponding setting from
     * the Tic’s non-volatile memory until the next Reset (or Reinitialize) command or full
     * microcontroller reset.
     *
     * @param maxSpeed Maximum speed.
     *   Range: 0 to 500,000,000
     *   Units: microsteps per 10,000 s
     * @return true if succeeded
     * @return false if failed
     */
    bool setMaxSpeed(uint32_t maxSpeed);

    /**
     * @brief This command temporarily sets the Tic’s starting speed in units of steps per 10,000
     * seconds. This is the maximum speed at which instant acceleration and deceleration are
     * allowed. The provided value will override the corresponding setting from the Tic’s
     * non-volatile memory until the next Reset (or Reinitialize) command or full microcontroller
     * reset.
     *
     * @param maxStartingSpeed Maximum starting speed.
     *   Range: 0 to 500,000,000
     *   Units: microsteps per 10,000 s
     * @return true if succeeded
     * @return false if failed
     */
    bool setStartingSpeed(uint32_t maxStartingSpeed);

    /**
     * @brief This command temporarily sets the Tic’s maximum allowed motor acceleration in units
     * of steps per second per 100 seconds. The provided value will override the corresponding
     * setting from the Tic’s non-volatile memory until the next Reset (or Reinitialize) command or
     * full microcontroller reset.
     *
     * @param maxAccel Maximum acceleration.
     *   Range: 100 to 2,147,483,647 = 0x64 to 0x7FFF FFFF
     *   Units: microsteps per 100 s2
     * @return true if succeeded
     * @return false if failed
     */
    bool setMaxAcceleration(uint32_t maxAccel);

    /**
     * @brief This command temporarily sets the Tic’s maximum allowed motor deceleration in units
     * of steps per second per 100 seconds. The provided value will override the corresponding
     * setting from the Tic’s non-volatile memory until the next Reset (or Reinitialize) command or
     * full microcontroller reset.
     *
     * @param maxDecel Maximum deceleration.
     *   Range: 100 to 2,147,483,647 = 0x64 to 0x7FFF FFFF
     *   Units: microsteps per 100 s2
     * @return true if succeeded
     * @return false if failed
     */
    bool setMaxDeceleration(uint32_t maxDecel);

    /**
     * @brief This command temporarily sets the step mode (also known as microstepping mode) of the
     * driver on the Tic, which defines how many microsteps correspond to one full step. The
     * provided value will override the corresponding setting from the Tic’s non-volatile memory
     * until the next Reset (or Reinitialize) command or full microcontroller reset.
     *
     * @param stepMode Step mode to set.
     * @return true if succeeded
     * @return false if failed
     */
    bool setStepMode(StepMode stepMode);

    /**
     * @brief This command reads a block of data from the Tic’s variables; the block starts from the
     * specified offset and can have a variable length
     *
     * @param variableOff Offset type of the variable to read out.
     * @param receiveData Buffer for the received data.
     * @return true if succeeded
     * @return false if failed
     */
    bool getVariable(VariableOffset variableOff, ByteBuffer& receiveData);

    /**
     * @brief This command is identical to the Get variable command, except that it also clears the
     * “Errors occurred” variable at the same time.
     *
     * @param variableOff Offset type of the variable to read out.
     * @param receiveData Buffer for the received data.
     * @return true if succeeded
     * @return false if failed
     */
    bool getVariableAndClearErrors(VariableOffset variableOff, ByteBuffer& receiveData);

    /**
     * @brief Checks if an error condition occurred.
     *
     * @return true Error condition occurred.
     * @return false No error condition occurred.
     */
    bool checkError();

private:
    bool runSimpleCommand(Byte commandId);

    I2cControl::Address m_address;
    I2cControl&         m_i2c;
    IGpioPin&           m_ioErr;
    IGpioPin&           m_ioRst;
    const std::string   m_me;
};

}  // namespace sugo::hal