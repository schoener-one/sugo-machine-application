# CMake configuration file
# Module: Protocol

find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(SRC_Protobuf HDR_Protobuf
    ${CMAKE_CURRENT_SOURCE_DIR}/Command.proto
)
get_filename_component(${HDR_Protobuf} DIR_Protobuf DIRECTORY)
add_library(Protocol STATIC ${SRC_Protobuf} ${HDR_Protobuf})
target_link_libraries(Protocol INTERFACE ${Protobuf_LIBRARIES})
target_include_directories (Protocol PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories (Protocol PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
