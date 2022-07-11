# CMake configuration file
# Module: HardwareServiceComponents

add_library (HardwareServiceComponents STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CircularMotionController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionSwitch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StepperMotor.cpp
    )
target_include_directories (HardwareServiceComponents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareServiceComponents Common HardwareAbstractionLayer)