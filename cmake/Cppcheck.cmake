## **********************************
## Adds cppcheck static code analysis
##
## **********************************

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    message("Static code analysis enabled for '${CPPCHECK_EXEC}'.")
    list(
        APPEND CMAKE_CXX_CPPCHECK
            "--quiet"
            "--enable=all"
            "--std=c++17" 
            "--inline-suppr"
            "--suppress=missingIncludeSystem"
    )
endif()
