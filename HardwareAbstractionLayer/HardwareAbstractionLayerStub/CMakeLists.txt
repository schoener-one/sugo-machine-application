# CMake configuration file
# Module: HardwareAbstractionLayerStub

add_library (HardwareAbstractionLayer STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GpioControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StepperMotorControl.cpp
    )
target_include_directories (HardwareAbstractionLayer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Common/include
    )
target_link_libraries(HardwareAbstractionLayer
    PUBLIC
    HardwareAbstractionLayerInterface
    PRIVATE
    HardwareAbstractionLayerCommon
    )

add_subdirectory(test)
