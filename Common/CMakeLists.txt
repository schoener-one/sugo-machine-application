# CMake configuration file
# Module: Common

add_library (Common STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigurationParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigurationFileParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Configuration.cpp
    )
target_include_directories (Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Common ${Boost_LIBRARIES})

# FIXME dependency to external libraries!
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_dependencies(Common jsonrpcpp_external)
endif()

# Common Mocks library
add_library (CommonMocks INTERFACE)
target_include_directories (CommonMocks INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
target_link_libraries(CommonMocks INTERFACE Common gmock)

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(CommonTest
        test/CommonTest.cpp
        test/ConfigurationTest.cpp
        test/CommandLineParserTest.cpp
        test/ConfigurationFileParserTest.cpp
        )
    target_compile_options(CommonTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(CommonTest
        Common
        CommonMocks
        ${Boost_LIBRARIES}
        pthread
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(CommonTest "" AUTO)
endif()