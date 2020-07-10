cmake_minimum_required(VERSION 3.3)
project(CoffeeMachine)

set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
message("Source path: ${PROJECT_SOURCE_DIR}")
message("Binary path: ${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi) # -Werror

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTARGET_X86")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTARGET_RPI")
endif()

if (NOT NO_UNITTEST)
    enable_testing()
    find_package(GTest REQUIRED)
endif()

## **********************************
## External libraries!
##

# AdafruiteStepperMotorHAT
add_library (AdafruitStepperMotorHAT 
    AdafruitStepperMotorHAT_CPP/Adafruit_MotorHAT.cpp 
    AdafruitStepperMotorHAT_CPP/PWM.cpp
    )
target_include_directories (AdafruitStepperMotorHAT PUBLIC ${PROJECT_SOURCE_DIR}/AdafruitStepperMotorHAT_CPP)
include_directories(${PROJECT_SOURCE_DIR}/AdafruitStepperMotorHAT_CPP)

# Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
add_definitions(-DBOOST_LOG_DYN_LINK)
set(Boost_MIN_VERSION "1.61")
find_package(Boost REQUIRED COMPONENTS log program_options system thread)
message ("Boost libraries: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})

## **********************************
## Tests
##
if (NOT NO_UNITTEST)
enable_testing()
endif()

## **********************************
## The MOCO project!
##

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayerX86)
    set(HardwareAbstractionLayerImpl HardwareAbstractionLayerX86)
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayerRpi)
    set(HardwareAbstractionLayerImpl HardwareAbstractionLayerRpi)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    target_link_libraries(AdafruitStepperMotorHAT ${HardwareAbstractionLayerImpl})
else()
    target_link_libraries(AdafruitStepperMotorHAT wiringPi)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/Common)
add_subdirectory(${PROJECT_SOURCE_DIR}/Protocol)
add_subdirectory(${PROJECT_SOURCE_DIR}/MessageBroker)
add_subdirectory(${PROJECT_SOURCE_DIR}/ServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/CoffeeAutomatService)
add_subdirectory(${PROJECT_SOURCE_DIR}/CupRotationTray)
add_subdirectory(${PROJECT_SOURCE_DIR}/CoffeeAutomatController)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer)
add_subdirectory(${PROJECT_SOURCE_DIR}/CoffeeAutomatApplication)

## **********************************
## Static code analysis
##
find_program(CPPCHECK_EXEC NAMES cppcheck)
if (CPPCHECK_EXEC)
    file(GLOB_RECURSE SRC_ALL 
        ${PROJECT_SOURCE_DIR}/CoffeeAutomatController/src/*.cpp
        ${PROJECT_SOURCE_DIR}/CoffeeAutomatApplication/src/*.cpp
        ${PROJECT_SOURCE_DIR}/CoffeeAutomatService/src/*.cpp
        ${PROJECT_SOURCE_DIR}/Common/src/*.cpp
        ${PROJECT_SOURCE_DIR}/ServiceComponents/src/*.cpp
        ${PROJECT_SOURCE_DIR}/CupRotationTray/src/*.cpp
        ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/src/*.cpp
        ${PROJECT_SOURCE_DIR}/HardwareComponents/src/*.cpp
        ${PROJECT_SOURCE_DIR}/MessageBroker/src/*.cpp
        )
    message("Static code analysis enabled for '${CPPCHECK_EXEC}'.")
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_EXEC}
        --quiet
        --enable=all
        --std=c++11
        --inline-suppr 
        --template "{file}:{line}: {severity} {id}: {message}"
        --suppress=missingIncludeSystem
        -I ${PROJECT_SOURCE_DIR}/CoffeeAutomatController/include
        -I ${PROJECT_SOURCE_DIR}/CoffeeAutomatApplication/include
        -I ${PROJECT_SOURCE_DIR}/CoffeeAutomatService/include
        -I ${PROJECT_SOURCE_DIR}/Common/include
        -I ${PROJECT_SOURCE_DIR}/ServiceComponents/include
        -I ${PROJECT_SOURCE_DIR}/CupRotationTray/include
        -I ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/include
        -I ${PROJECT_SOURCE_DIR}/HardwareComponents/include
        -I ${PROJECT_SOURCE_DIR}/MessageBroker/include
        ${SRC_ALL}
    )
    add_dependencies(CoffeeAutomatApplication cppcheck)
endif()
