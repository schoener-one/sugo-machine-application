# CMake configuration file
# Module: CupRotationTray

add_library (CupRotationTray STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CupRotationTray.cpp
    )
target_include_directories (CupRotationTray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CupRotationTray
    Common
    Protocol
    MessageBroker
    HardwareComponents
    )