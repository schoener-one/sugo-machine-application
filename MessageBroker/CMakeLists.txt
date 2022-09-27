# CMake configuration file
# Module: MessageBroker

add_library (MessageBroker STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommandMessageBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IOContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ProcessContext.cpp
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

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(MessageBrokerTest
        test/CommandMessageBrokerIntegrationTest.cpp
        test/ClientServerIntegrationTest.cpp
        )
    target_compile_options(MessageBrokerTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(MessageBrokerTest
        Common
        Protocol
        MessageBroker
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(MessageBrokerTest "" AUTO)
endif()