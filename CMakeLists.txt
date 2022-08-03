cmake_minimum_required(VERSION 3.3)
project(SugoMachine)

set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
message("Source path: ${PROJECT_SOURCE_DIR}")
message("Binary path: ${PROJECT_BINARY_DIR}")

include(cmake/CompilerConfig.cmake)
include(cmake/Coverage.cmake)
include(cmake/Testing.cmake)
include(cmake/GenerateServiceComponents.cmake)

## **********************************
## Sub directories
##
list(APPEND PROJECT_SUBDIRS 
    ${PROJECT_SOURCE_DIR}/Common
    ${PROJECT_SOURCE_DIR}/Protocol
    ${PROJECT_SOURCE_DIR}/MessageBroker
    ${PROJECT_SOURCE_DIR}/ServiceComponents
    ${PROJECT_SOURCE_DIR}/MachineServiceGateway
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer
    ${PROJECT_SOURCE_DIR}/MachineServiceComponents
    ${PROJECT_SOURCE_DIR}/MachineApplication
    )

# FIXME remove globbing!
file(GLOB_RECURSE SRC_ALL 
    ${PROJECT_SOURCE_DIR}/MachineController/*.cpp
    ${PROJECT_SOURCE_DIR}/MachineApplication/*.cpp
    ${PROJECT_SOURCE_DIR}/MachineServiceGateway/*.cpp
    ${PROJECT_SOURCE_DIR}/Common/*.cpp
    ${PROJECT_SOURCE_DIR}/ServiceComponents/*.cpp
    ${PROJECT_SOURCE_DIR}/CupRotationTray/*.cpp
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/*.cpp
    ${PROJECT_SOURCE_DIR}/HardwareServiceComponents/*.cpp
    ${PROJECT_SOURCE_DIR}/MessageBroker/*.cpp
)
file(GLOB_RECURSE HDR_ALL 
    ${PROJECT_SOURCE_DIR}/MachineController/*.hpp
    ${PROJECT_SOURCE_DIR}/MachineApplication/*.hpp
    ${PROJECT_SOURCE_DIR}/MachineServiceGateway/*.hpp
    ${PROJECT_SOURCE_DIR}/Common/*.hpp
    ${PROJECT_SOURCE_DIR}/ServiceComponents/*.hpp
    ${PROJECT_SOURCE_DIR}/CupRotationTray/*.hpp
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/*.hpp
    ${PROJECT_SOURCE_DIR}/HardwareServiceComponents/*.hpp
    ${PROJECT_SOURCE_DIR}/MessageBroker/*.hpp
)

## **********************************
## External libraries!
##
include(cmake/ExternalLib.cmake)

# Boost library
include(cmake/LibBoost.cmake)

## **********************************
## The Sugo project!
##
# TODO use automatic add_subdirectory!
# foreach(subdir ${PROJECT_SUBDIRS})
#     add_subdirectory(${subdir})
# endforeach()

add_subdirectory(${PROJECT_SOURCE_DIR}/Common)
add_subdirectory(${PROJECT_SOURCE_DIR}/Protocol)
add_subdirectory(${PROJECT_SOURCE_DIR}/MessageBroker)
add_subdirectory(${PROJECT_SOURCE_DIR}/ServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineServiceGateway)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineApplication)

include(cmake/Cppcheck.cmake)
include(cmake/ClangFormatCheck.cmake)