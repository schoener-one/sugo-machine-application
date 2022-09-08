# CMake configuration file
# Module: MachineServiceGateway

add_library(MachineServiceGateway STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineServiceGateway.cpp
    )
target_include_directories(MachineServiceGateway PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MachineServiceGateway
    Common
    Protocol
    MessageBroker
    ServiceComponents
    zmq
    )

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(MachineServiceGateway jsonrpcpp_external)
endif()