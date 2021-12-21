## **********************************
## Adds coverage analysis
##
## **********************************

if (CODE_COVERAGE)
    message("Creating coverage build")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")

    add_custom_target(
        coverage_html
        COMMAND 
        gcovr -r ${PROJECT_SOURCE_DIR} --exclude '${PROJECT_SOURCE_DIR}/build.*/' --html=coverage.html --html-details
    )
    add_custom_target(
        coverage_xml
        COMMAND 
        gcovr -r ${PROJECT_SOURCE_DIR} --exclude '${PROJECT_SOURCE_DIR}/build.*/' --xml=coverage.xml --xml-pretty
    )
endif()
