# CMake configuration file
# Module: CoffeeAutomatApplication

add_library (CoffeeAutomatApplication STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoffeeAutomatApplication.cpp
    )
target_include_directories (CoffeeAutomatApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CoffeeAutomatApplication
    Common
    Protocol
    MessageBroker
    CoffeeAutomatService
    CupRotationTray
    CoffeeAutomat
    HardwareComponents
    HardwareAbstractionLayer
    )