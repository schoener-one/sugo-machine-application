set(MODULE MessageBroker)

add_library (${MODULE} STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommandMessageBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IOContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ProcessContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Publisher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Message.cpp
    )
target_include_directories (${MODULE} PUBLIC include)
target_link_libraries(${MODULE}
    PUBLIC
    Common
    Protocol
    zmq
    )

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(${MODULE} azmq_external)
endif()

# Common Mocks library
add_library (${MODULE}Mocks INTERFACE)
target_include_directories (${MODULE}Mocks INTERFACE mocks)
target_link_libraries(${MODULE}Mocks INTERFACE
    ${MODULE}
    gmock
    )

if (NOT NO_UNITTEST)
    add_subdirectory(test)
endif()
