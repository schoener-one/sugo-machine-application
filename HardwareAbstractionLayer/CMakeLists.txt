# CMake configuration file
# Module: HardwareAbstractionLayer

add_library (HardwareAbstractionLayer STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdafruitStepperMotorController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IHardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SysfsGpioController.cpp
    )
target_include_directories (HardwareAbstractionLayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayer Common AdafruitStepperMotorHAT)