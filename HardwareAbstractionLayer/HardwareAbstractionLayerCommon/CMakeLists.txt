# CMake configuration file
# Module: HardwareAbstractionLayerCommon

add_library (HardwareAbstractionLayerCommon STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IHardwareAbstractionLayer.cpp
    )
target_include_directories (HardwareAbstractionLayerCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayerCommon 
    PUBLIC
    HardwareAbstractionLayerInterface
    Common
    )