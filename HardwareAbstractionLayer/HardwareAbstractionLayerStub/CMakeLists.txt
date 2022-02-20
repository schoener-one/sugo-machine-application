# CMake configuration file
# Module: HardwareAbstractionLayerStub

add_library (HardwareAbstractionLayer STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IHardwareAbstractionLayer.cpp
    )
target_include_directories (HardwareAbstractionLayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayer PUBLIC HardwareAbstractionLayerInterface)
