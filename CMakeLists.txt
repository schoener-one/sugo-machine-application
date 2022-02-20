cmake_minimum_required(VERSION 3.3)
project(SugoMachine)

set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
message("Source path: ${PROJECT_SOURCE_DIR}")
message("Binary path: ${PROJECT_BINARY_DIR}")

include(cmake/CompilerConfig.cmake)
include(cmake/Coverage.cmake)
include(cmake/Testing.cmake)

## **********************************
## Sub directories
##
list(APPEND PROJECT_SUBDIRS 
    ${PROJECT_SOURCE_DIR}/Common
    ${PROJECT_SOURCE_DIR}/Protocol
    ${PROJECT_SOURCE_DIR}/MessageBroker
    ${PROJECT_SOURCE_DIR}/ServiceComponents
    ${PROJECT_SOURCE_DIR}/MachineService
    ${PROJECT_SOURCE_DIR}/CupRotationTray
    ${PROJECT_SOURCE_DIR}/MachineController
    ${PROJECT_SOURCE_DIR}/HardwareComponents
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer
    ${PROJECT_SOURCE_DIR}/MachineApplication
    )

## **********************************
## External libraries!
##
include(cmake/ExternalLib.cmake)

# Boost library
include(cmake/LibBoost.cmake)

## **********************************
## The MOCO project!
##
# TODO use automatic add_subdirectory!
# foreach(subdir ${PROJECT_SUBDIRS})
#     add_subdirectory(${subdir})
# endforeach()

add_subdirectory(${PROJECT_SOURCE_DIR}/Common)
add_subdirectory(${PROJECT_SOURCE_DIR}/Protocol)
add_subdirectory(${PROJECT_SOURCE_DIR}/MessageBroker)
add_subdirectory(${PROJECT_SOURCE_DIR}/ServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineService)
add_subdirectory(${PROJECT_SOURCE_DIR}/CupRotationTray)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineController)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineApplication)

include(cmake/Cppcheck.cmake)
include(cmake/ClangFormatCheck.cmake)