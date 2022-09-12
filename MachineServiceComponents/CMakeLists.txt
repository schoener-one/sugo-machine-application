# CMake configuration file
# Module: ${MODULE_NAME}
set(MODULE_NAME "MachineServiceComponents")
generate_service_components(
    TARGET ${MODULE_NAME}
    CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/MachineServiceComponents.yaml"
    )
generate_service_components_exec_group(
    TARGET ${MODULE_NAME}
    CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/MachineServiceComponents.yaml"
    )
add_library(${MODULE_NAME}
    src/FilamentCoilControl.cpp
    src/FilamentCoilMotor.cpp
    src/FilamentFeederMotor.cpp
    src/FilamentMergerControl.cpp
    src/FilamentMergerHeater.cpp
    src/FilamentPreHeater.cpp
    src/FilamentTensionSensor.cpp
    src/MachineControl.cpp
    src/UserInterfaceControl.cpp
    src/HeaterService.cpp
    src/MotorService.cpp
    )
target_include_directories (${MODULE_NAME}
    PUBLIC
    include
    )
target_link_libraries(${MODULE_NAME} PUBLIC
    Common
    Protocol
    MessageBroker
    MachineServiceGateway
    ${MODULE_NAME}-GenLib
    HardwareAbstractionLayer
    MachineRemoteControl
    )
 
if (NOT NO_UNITTEST)
    add_subdirectory(test)
endif()
