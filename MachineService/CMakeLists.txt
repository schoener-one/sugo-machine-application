# CMake configuration file
# Module: MachineService

add_library (MachineService STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineService.cpp
    )
target_include_directories (MachineService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MachineService
    Common
    Protocol
    MessageBroker
    ServiceComponents
    zmq
    )

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(MachineService jsonrpcpp)
endif()