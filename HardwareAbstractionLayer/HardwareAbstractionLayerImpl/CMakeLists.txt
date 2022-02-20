# CMake configuration file
# Module: HardwareAbstractionLayerImpl

add_library (HardwareAbstractionLayer STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SysfsGpioController.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayer PUBLIC HardwareAbstractionLayerInterface)