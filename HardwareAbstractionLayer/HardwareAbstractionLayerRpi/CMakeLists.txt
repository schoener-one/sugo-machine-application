# CMake configuration file
# Module: HardwareAbstractionLayerRpi

add_library (HardwareAbstractionLayer STATIC 
    src/GpioControl.cpp
    src/AdcControl.cpp
    src/StepperMotorControl.cpp
    src/GpioPin.cpp
    src/AdcInput.cpp
    src/StepperMotor.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE include)
target_link_libraries(HardwareAbstractionLayer
    PUBLIC
    HardwareAbstractionLayerInterface
    Common
    gpiod
    wsadhat
    )

add_subdirectory(test)