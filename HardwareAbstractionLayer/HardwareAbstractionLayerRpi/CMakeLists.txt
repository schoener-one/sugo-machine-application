# CMake configuration file
# Module: HardwareAbstractionLayerRpi

add_library (HardwareAbstractionLayer STATIC 
    src/GpioControl.cpp
    src/AdcControl.cpp
    src/StepperMotorControl.cpp
    src/GpioPin.cpp
    src/AdcInput.cpp
    src/StepperMotor.cpp
    external/libwsadhat/src/AdcHat.cpp
    external/libwsadhat/src/SpiControl.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE
    include
    external/libwsadhat/include
    )
target_link_libraries(HardwareAbstractionLayer
    PUBLIC
    HardwareAbstractionLayerInterface
    Common
    gpiod
    gpiodcxx
    )

add_subdirectory(test)