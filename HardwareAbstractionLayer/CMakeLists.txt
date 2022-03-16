# CMake configuration file
# Module: HardwareAbstractionLayer

add_library(HardwareAbstractionLayerInterface INTERFACE)
target_include_directories (HardwareAbstractionLayerInterface INTERFACE include)
target_link_libraries(HardwareAbstractionLayerInterface INTERFACE Common)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common/include)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    message("Using HardwareAbstractionLayerStub (x86)")
    add_subdirectory(HardwareAbstractionLayerStub)
else()
    message("Using HardwareAbstractionLayerRpi")
    add_subdirectory(HardwareAbstractionLayerRpi)
endif()

target_include_directories(HardwareAbstractionLayer PRIVATE Common/include)
target_sources(HardwareAbstractionLayer 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/src/IHardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/src/HardwareAbstractionLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/src/AdcFilter.cpp
    )