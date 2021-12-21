# CMake configuration file
# Module: MachineController

add_library (MachineController STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineController.cpp
    )
target_include_directories (MachineController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MachineController
    Common
    ServiceComponents
    Protocol
    MessageBroker
    CupRotationTray
    )
