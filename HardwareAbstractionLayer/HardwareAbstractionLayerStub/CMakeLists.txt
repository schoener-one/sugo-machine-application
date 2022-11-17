# CMake configuration file
# Module: HardwareAbstractionLayerStub

add_library (HardwareAbstractionLayer STATIC 
    src/GpioControl.cpp
    src/TemperatureSensorControl.cpp
    src/StepperMotorControl.cpp
    src/GpioPin.cpp
    src/TemperatureSensor.cpp
    src/StepperMotor.cpp
    src/Simulator.cpp
    src/HardwareAbstractionLayer.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE
    include
    )
target_link_libraries(HardwareAbstractionLayer
    PUBLIC
    HardwareAbstractionLayerInterface
    Common
    )

add_subdirectory(test)
