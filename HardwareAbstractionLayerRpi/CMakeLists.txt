# CMake configuration file
# Module: HardwareAbstractionLayerRpi

add_library (HardwareAbstractionLayerRpi STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SysfsGpioController.cpp
    )
target_include_directories (HardwareAbstractionLayerRpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayerRpi Common HardwareAbstractionLayer)