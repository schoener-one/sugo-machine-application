## **********************************
## Adds boost library
##
## **********************************

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_LOG_DYN_LINK)
set(Boost_MIN_VERSION "1.61")
find_package(Boost REQUIRED COMPONENTS log program_options system thread)
message ("Boost libraries: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})