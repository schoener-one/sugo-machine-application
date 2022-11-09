cmake_minimum_required(VERSION 3.22)
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
## External libraries!
##
include(cmake/ExternalLib.cmake)

# Add boost library
include(cmake/LibBoost.cmake)
# Enable cppcheck
include(cmake/Cppcheck.cmake)
# Enable clang-format check
include(cmake/ClangFormatCheck.cmake)

## **********************************
## The Sugo machine project!
##
add_subdirectory(${PROJECT_SOURCE_DIR}/Common)
add_subdirectory(${PROJECT_SOURCE_DIR}/MessageBroker)
add_subdirectory(${PROJECT_SOURCE_DIR}/ServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineServiceGateway)
add_subdirectory(${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineServiceComponents)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineApplication)
add_subdirectory(${PROJECT_SOURCE_DIR}/MachineRemoteControl)
