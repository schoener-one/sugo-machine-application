# CMake configuration file
# Module: HardwareAbstractionLayer

# HardwareAbstractionLayerInterface library
add_library(HardwareAbstractionLayerInterface INTERFACE)
target_include_directories (HardwareAbstractionLayerInterface INTERFACE include)
target_link_libraries(HardwareAbstractionLayerInterface INTERFACE Common)

# HardwareAbstractionLayer library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common/include)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_compile_definitions(HAL_SIMULATION_ENABLE)
    message("Using HardwareAbstractionLayerStub (x86)")
    add_subdirectory(HardwareAbstractionLayerStub)
else()
    message("Using HardwareAbstractionLayerRpi")
    add_subdirectory(HardwareAbstractionLayerRpi)
endif()

target_include_directories(HardwareAbstractionLayer PRIVATE Common/include)
target_sources(HardwareAbstractionLayer 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/src/HardwareAbstractionLayer.cpp
    )
add_subdirectory(test)

# HardwareAbstractionLayerInterfaceMock library
add_library(HardwareAbstractionLayerInterfaceMocks INTERFACE)
target_include_directories (HardwareAbstractionLayerInterfaceMocks INTERFACE mocks)
target_link_libraries(HardwareAbstractionLayerInterfaceMocks INTERFACE
    HardwareAbstractionLayerInterface
    )