cmake_minimum_required(VERSION 3.10)
project(BOMBERBLOKE)

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}"/cmake")

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIRS})
include_directories("include")



# Client setup
set(client_src "src/client.cpp")
add_executable(client ${client_src})
target_link_libraries(client ${ENET_LIBRARIES})

# Server setup
set(server_src "src/server.cpp")
add_executable(server ${server_src})
target_link_libraries(server ${ENET_LIBRARIES})

# Format in clang
file(GLOB_RECURSE all_sources include/*.h src/*.cpp)
add_custom_target(fmt
    COMMAND clang-format -style=Mozzila -i ${all_sources})



# Warning messages are treated as errors
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR}/resources/)
