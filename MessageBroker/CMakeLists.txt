# CMake configuration file
# Module: MessageBroker

add_library (MessageBroker STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommandMessageBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IOContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ProcessContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Publisher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Message.cpp
    )
target_include_directories (MessageBroker PUBLIC include)
target_link_libraries(MessageBroker
    PUBLIC
    Common
    Protocol
    zmq
    )

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(MessageBroker azmq_external)
endif()

# Common Mocks library
add_library (MessageBrokerMocks INTERFACE)
target_include_directories (MessageBrokerMocks INTERFACE mocks)
target_link_libraries(MessageBrokerMocks INTERFACE
    MessageBroker
    gmock
    )

if (NOT NO_UNITTEST)
    add_subdirectory(test)
endif()
