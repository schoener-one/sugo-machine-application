///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-28
 */
///////////////////////////////////////////////////////////////////////////////

#include "TicController.hpp"
#include "HalTypes.hpp"
#include "Logger.hpp"

#include <bitset>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <thread>

namespace
{
constexpr char Me[] = "TicController";

template <class ValueT = int32_t>
constexpr void writeToBuffer(ValueT value, sugo::hal::ByteBuffer& buffer, unsigned offset)
{
    buffer[0 + offset] = static_cast<sugo::hal::Byte>(value >> 0 & 0xff);
    buffer[1 + offset] = static_cast<sugo::hal::Byte>(value >> 8 & 0xff);
    buffer[2 + offset] = static_cast<sugo::hal::Byte>(value >> 16 & 0xff);
    buffer[3 + offset] = static_cast<sugo::hal::Byte>(value >> 24 & 0xff);
}

enum OperationState
{
    Reset             = 0,
    DeEnergize        = 2,
    SoftError         = 4,
    WaitingForErrLine = 6,
    StartingUp        = 8,
    Normal            = 10
};

const std::map<sugo::hal::Byte, std::string> s_operationStateName = {
    {static_cast<sugo::hal::Byte>(OperationState::Reset), "Reset"},
    {static_cast<sugo::hal::Byte>(OperationState::DeEnergize), "DeEnergize"},
    {static_cast<sugo::hal::Byte>(OperationState::SoftError), "SoftError"},
    {static_cast<sugo::hal::Byte>(OperationState::WaitingForErrLine), "WaitingForErrLine"},
    {static_cast<sugo::hal::Byte>(OperationState::StartingUp), "StartingUp"},
    {static_cast<sugo::hal::Byte>(OperationState::Normal), "Normal"},
};
}  // namespace

using namespace sugo::hal;

TicController::TicController(I2cControl::Address address, I2cControl& i2cControl, IGpioPin& ioErr,
                             IGpioPin& ioRst)
    : m_address(address),
      m_i2c(i2cControl),
      m_ioErr(ioErr),
      m_ioRst(ioRst),
      m_me(std::string(Me) + "(" + std::to_string(static_cast<unsigned>(m_address)) + "): ")
{
}

TicController::~TicController()
{
    finalize();
}

bool TicController::init()
{
    m_ioRst.setState(IGpioPin::State::Low);                      // turn off
    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // at least 10ms
    m_ioRst.setState(IGpioPin::State::High);                     // turn on
    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // wait for restart finished

    LOG(debug) << m_me << "Init device";

    if (!reset())
    {
        LOG(error) << m_me << "Failed to reset device";
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // wait for reset finished

    // clear position
    if (!haltAndSetPosition(0))
    {
        LOG(error) << m_me << "Failed to halt and set position on device";
        return false;
    }

    (void)setTargetPosition(0);
    (void)setTargetVelocity(0);
    (void)setStepMode(StepMode::Step1_8);
    (void)exitSafeStart();

    ByteBuffer data(1);
    if (!getVariableAndClearErrors(VariableOffset::OperationState, data))
    {
        LOG(error) << m_me << "Failed to get operation state on device";
        return false;
    }

    if (data.at(0) != static_cast<Byte>(OperationState::Normal))
    {
        LOG(warning) << m_me << "Bad operation state: " << s_operationStateName.at(data.at(0));
        checkError();
    }

    return true;
}

void TicController::finalize()
{
    if (m_ioRst.getState() == IGpioPin::State::High)
    {
        m_ioRst.setState(IGpioPin::State::Low);  // turn off
    }
}

bool TicController::setTargetPosition(int32_t position)
{
    static constexpr Byte commandId = 0xE0;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(position, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);

    return success;
}

bool TicController::setTargetVelocity(int32_t velocity)
{
    static constexpr Byte commandId = 0xE3;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(velocity, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);

    return success;
}

bool TicController::haltAndSetPosition(int32_t position)
{
    static constexpr Byte commandId = 0xEC;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(position, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);

    return success;
}

bool TicController::haltAndHold()
{
    static constexpr Byte commandId = 0x89;
    return runSimpleCommand(commandId);
}

bool TicController::goHome(IStepperMotor::Direction direction)
{
    static constexpr Byte commandId = 0x97;
    ByteBuffer            commandData{commandId, static_cast<Byte>(direction)};
    const bool            success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::resetCommandTimeout()
{
    static constexpr Byte commandId = 0x8C;
    return runSimpleCommand(commandId);
}

bool TicController::deEnergize()
{
    static constexpr Byte commandId = 0x86;
    return runSimpleCommand(commandId);
}

bool TicController::energize()
{
    static constexpr Byte commandId = 0x85;
    return runSimpleCommand(commandId);
}

bool TicController::exitSafeStart()
{
    static constexpr Byte commandId = 0x83;
    return runSimpleCommand(commandId);
}

bool TicController::enterSafeStart()
{
    static constexpr Byte commandId = 0x8F;
    return runSimpleCommand(commandId);
}

bool TicController::reset()
{
    static constexpr Byte commandId = 0xB0;
    return runSimpleCommand(commandId);
}

bool TicController::setMaxSpeed(uint32_t maxSpeed)
{
    static constexpr Byte commandId = 0xE6;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(maxSpeed, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::setStartingSpeed(uint32_t maxStartingSpeed)
{
    static constexpr Byte commandId = 0xE5;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(maxStartingSpeed, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::setMaxAcceleration(uint32_t maxAccel)
{
    static constexpr Byte commandId = 0xEA;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(maxAccel, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::setMaxDeceleration(uint32_t maxDecel)
{
    static constexpr Byte commandId = 0xE9;
    ByteBuffer            commandData{commandId, 0, 0, 0, 0};
    writeToBuffer(maxDecel, commandData, sizeof(commandId));
    bool success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::setStepMode(StepMode stepMode)
{
    static constexpr Byte commandId = 0x94;
    ByteBuffer            commandData{commandId, static_cast<Byte>(stepMode)};
    const bool            success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::getVariable(VariableOffset variableOff, ByteBuffer& receiveData)
{
    static constexpr Byte commandId = 0xA1;
    ByteBuffer            commandData{commandId, static_cast<Byte>(variableOff)};
    const bool            success = m_i2c.read(m_address, commandData, receiveData);
    return success;
}

bool TicController::getVariableAndClearErrors(VariableOffset variableOff, ByteBuffer& receiveData)
{
    static constexpr Byte commandId = 0xA2;
    ByteBuffer            commandData{commandId, static_cast<Byte>(variableOff)};
    const bool            success = m_i2c.read(m_address, commandData, receiveData);
    return success;
}

bool TicController::runSimpleCommand(Byte commandId)
{
    ByteBuffer commandData{commandId};
    const bool success = m_i2c.write(m_address, commandData);
    return success;
}

bool TicController::checkError()
{
    bool errorState = false;
    if (m_ioErr.getState() == IGpioPin::State::High)
    {
        errorState = true;
        ByteBuffer  buffer(2);
        const bool  success = getVariable(VariableOffset::ErrorStatus, buffer);
        std::string errors;
        if (success)
        {
            const std::bitset<8> bit(buffer.at(0));
            if (bit[0])
            {
                errors += "INTENT_DEENERGIZED ";
            }
            else if (bit[1])
            {
                errors += "MOTOR_DRIVE_ERROR ";
            }
            else if (bit[2])
            {
                errors += "LOW_VIN ";
            }
            else if (bit[3])
            {
                errors += "KILL_SWITCH_ACTIVE ";
            }
            else if (bit[4])
            {
                errors += "REQ_INPUT_INVALID ";
            }
            else if (bit[5])
            {
                errors += "SERIAL_ERROR ";
            }
            else if (bit[6])
            {
                errors += "COMMAND_TIMEOUT ";
            }
            else if (bit[7])
            {
                errors += "SAVE_START_VIOLATION ";
            }
            else
            {
                errorState = false;
                LOG(warning) << m_me << "Error line high, but no error condition!";
            }
        }
        else
        {
            errors = "UNKNOWN";
        }

        if (errorState)
        {
            const Byte flags = buffer.at(1);
            LOG(error) << m_me << "Error state: " << errors << " (" << std::hex << std::setw(2)
                       << std::setfill('0') << static_cast<unsigned>(flags) << ")";
        }
    }
    return !errorState;
}