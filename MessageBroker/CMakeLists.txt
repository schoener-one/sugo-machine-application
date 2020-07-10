# CMake configuration file
# Module: MessageBroker

add_library (MessageBroker STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Responder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Requester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommandMessageBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IOContext.cpp
    )
target_include_directories (MessageBroker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MessageBroker Common Protocol)

# Common Mocks library
add_library (MessageBrokerMocks INTERFACE)
target_include_directories (MessageBrokerMocks INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/test)
target_link_libraries(MessageBrokerMocks INTERFACE MessageBroker gmock)

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(MessageBrokerTest
        test/CommandMessageBrokerTest.cpp
        test/RequesterResponderTest.cpp
        )
    target_compile_options(MessageBrokerTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(MessageBrokerTest
        Common
        Protocol
        MessageBroker
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        zmq
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(MessageBrokerTest "" AUTO)
endif()