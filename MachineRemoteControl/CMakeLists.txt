# CMake configuration file

set(MODULE_NAME "MachineRemoteControl")

add_library (${MODULE_NAME} STATIC 
    src/RemoteControlServer.cpp
    src/Connection.cpp
    )
target_include_directories (${MODULE_NAME} PUBLIC include)
target_link_libraries(${MODULE_NAME}
    PUBLIC
    Common
    PRIVATE
    mongoose
    )

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(${MODULE_NAME} mongoose_external)
endif()

add_executable(${MODULE_NAME}Server
    test/main.cpp
)
target_link_libraries(${MODULE_NAME}Server PRIVATE ${MODULE_NAME})