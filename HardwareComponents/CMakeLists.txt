# CMake configuration file
# Module: HardwareComponents

add_library (HardwareComponents STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CircularMotionController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionSwitch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StepperMotor.cpp
    )
target_include_directories (HardwareComponents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareComponents Common HardwareAbstractionLayer)