## **********************************
## Adds clang-format check
##
## **********************************

# FIXME remove globbing!
file(GLOB_RECURSE SOURCE_ALL 
    ${PROJECT_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCE_ALL EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*" )
file(GLOB_RECURSE HEADER_ALL 
    ${PROJECT_SOURCE_DIR}/*.hpp
)
list(FILTER HEADER_ALL EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*" )

set(CLANG_FORMAT_LOG ${CMAKE_CURRENT_BINARY_DIR}/clang-format.log)
add_custom_command(
    OUTPUT            ${CLANG_FORMAT_LOG}
    COMMENT           "Checking clang-format"
    COMMAND           clang-format --dry-run --Werror ${SOURCE_ALL} ${HEADER_ALL} 2> ${CLANG_FORMAT_LOG}
    )

add_custom_target(clang_format_check
    ALL
    DEPENDS ${CLANG_FORMAT_LOG})