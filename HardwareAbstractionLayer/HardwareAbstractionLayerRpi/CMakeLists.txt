# CMake configuration file
# Module: HardwareAbstractionLayerRpi

add_library (HardwareAbstractionLayer STATIC 
    src/GpioControl.cpp
    src/TemperatureSensorControl.cpp
    src/StepperMotorControl.cpp
    src/GpioPin.cpp
    src/TemperatureSensor.cpp
    src/StepperMotor.cpp
    src/Max31865.cpp
    src/SpiControl.cpp
    src/I2cControl.cpp
    src/TicController.cpp
    src/StepperMotor.cpp
    src/HardwareAbstractionLayer.cpp
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