# CMake configuration file
# Module: HardwareAbstractionLayer

add_library(HardwareAbstractionLayerInterface INTERFACE)
target_include_directories (HardwareAbstractionLayerInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HardwareAbstractionLayerInterface INTERFACE Common)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HardwareAbstractionLayerStub)
else()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HardwareAbstractionLayerImpl)
endif()
