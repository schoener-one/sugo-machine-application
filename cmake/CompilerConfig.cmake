## **********************************
## Sets the compiler flags
##
## **********************************

add_compile_options(
    -pipe
    -g
    $<$<CONFIG:DEBUG>:-ggdb>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O2>
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi -Werror)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_compile_definitions(TARGET_X86)
else()
    add_compile_definitions(TARGET_RPI)
endif()