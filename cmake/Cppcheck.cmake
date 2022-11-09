## **********************************
## Adds cppcheck static code analysis
##
## **********************************

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    message("Static code analysis enabled for sources")
    list(APPEND CMAKE_CXX_CPPCHECK
        "--std=c++17" 
        "--quiet"
        "--enable=all"
        "--inline-suppr"
        "--suppress=missingIncludeSystem"
        "--suppress=unusedFunction"
        "--suppress=unmatchedSuppression"
        "--suppress=*:${CMAKE_BINARY_DIR}/*"
        "--template=!CPPCHECK: {file}:{line}: {id} ({severity}) - {message}"
    )
endif()
