# CMake configuration file

set(MODULE_NAME "ServiceComponents")
add_library (${MODULE_NAME} STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ServiceComponent.cpp
    )
target_include_directories(${MODULE_NAME} PUBLIC include)
target_link_libraries(${MODULE_NAME}
    PUBLIC
    Common
    MessageBroker
    )

# Mocks library
add_library (${MODULE_NAME}Mocks INTERFACE)
target_include_directories (${MODULE_NAME}Mocks INTERFACE mocks)
target_link_libraries(${MODULE_NAME}Mocks INTERFACE Common gmock)

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(${MODULE_NAME}Test
        test/StatedServiceComponentTest.cpp
        test/EventQueueTest.cpp
        test/StatedServiceComponentIntegrationTest.cpp
    )
    target_include_directories (${MODULE_NAME} PUBLIC mocks)
    target_compile_options(${MODULE_NAME}Test PUBLIC "-DUNIT_TEST")
    target_link_libraries(${MODULE_NAME}Test
        Common
        CommonMocks
        MessageBrokerMocks
        ${MODULE_NAME}
        ${MODULE_NAME}Mocks
        ${Boost_LIBRARIES}
        pthread
        GTest::GTest
        GTest::Main
        gmock
    )
    gtest_add_tests(${MODULE_NAME}Test "" AUTO)
endif()