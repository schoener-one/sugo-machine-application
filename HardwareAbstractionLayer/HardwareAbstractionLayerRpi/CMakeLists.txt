# CMake configuration file
# Module: HardwareAbstractionLayerRpi

add_library (HardwareAbstractionLayer STATIC 
    src/GpioControl.cpp
    src/AdcControl.cpp
    src/StepperMotorControl.cpp
    src/GpioPin.cpp
    src/AdcInput.cpp
    src/StepperMotor.cpp
    src/AdcHat.cpp
    src/SpiControl.cpp
    src/I2cControl.cpp
    src/TicController.cpp
    src/StepperMotor.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE
    include
    )
target_link_libraries(HardwareAbstractionLayer
    PUBLIC
    HardwareAbstractionLayerInterface
    Common
    gpiod
    gpiodcxx
    )

add_subdirectory(test)