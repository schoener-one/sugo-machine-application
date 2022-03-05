## **********************************
## Sets the compiler flags
##
## **********************************

set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi) # TODO -Werror
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTARGET_X86")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTARGET_RPI")
endif()