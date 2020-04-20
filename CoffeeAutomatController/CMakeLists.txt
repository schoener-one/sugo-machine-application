# CMake configuration file
# Module: CoffeeAutomatController

add_library (CoffeeAutomatController STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoffeeAutomatController.cpp
    )
target_include_directories (CoffeeAutomatController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CoffeeAutomatController
    Common
    ServiceComponents
    Protocol
    MessageBroker
    CupRotationTray
    )
