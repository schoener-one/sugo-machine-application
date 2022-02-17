## **********************************
## Adds clang-format check
##
## **********************************

set(CLANG_FORMAT_LOG ${CMAKE_CURRENT_BINARY_DIR}/clang-format.log)
add_custom_command(
    OUTPUT            ${CLANG_FORMAT_LOG}
    COMMENT           "Checking clang-format"
    COMMAND           clang-format --dry-run --Werror ${SRC_ALL} ${HDR_ALL} 2> ${CLANG_FORMAT_LOG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

add_custom_target(clang_format_check
    ALL
    DEPENDS ${CLANG_FORMAT_LOG})