cmake_minimum_required(VERSION 3.10)
project(BOMBERBLOKE)

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIRS})
include_directories("include")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#Include directories
include_directories(${PROJECT_SOURCE_DIR}/engine/event)
include_directories(${PROJECT_SOURCE_DIR}/engine/file)
include_directories(${PROJECT_SOURCE_DIR}/engine/level)
include_directories(${PROJECT_SOURCE_DIR}/engine/network)
include_directories(${PROJECT_SOURCE_DIR}/engine/player)
include_directories(${PROJECT_SOURCE_DIR}/engine/actor)
include_directories(${PROJECT_SOURCE_DIR}/engine/control)
include_directories(${PROJECT_SOURCE_DIR}/client)
include_directories(${PROJECT_SOURCE_DIR}/common)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/client_sources.cmake CLIENT_SOURCES)
add_library(${LIB_TARGET_NAME} CLIENT_ENGINE
            ${CLIENT_SOURCES}
            )


# Client setup
set(client_src "client/bomberbloke.cpp")

add_executable(bomberbloke_client ${client_src})
set_target_properties(bomberbloke_client PROPERTIES PUBLIC_HEADER "engine/client.h" )
target_link_libraries(bomberbloke_client ${ENET_LIBRARIES} ${SDL2_LIBRARIES} CLIENT_ENGINE)


# Server setup
#set(server_src "engine/server.cpp")
#add_executable(server ${server_src})
#target_link_libraries(server ${ENET_LIBRARIES} ${SDL2_LIBRARY})

# Format in clang
file(GLOB_RECURSE all_sources include/*.h src/*.cpp)
add_custom_target(fmt
    COMMAND clang-format -style=Mozzila -i ${all_sources})



# Warning messages are treated as errors
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")

# file(COPY
#     ${CMAKE_CURRENT_SOURCE_DIR}/resources/
#     DESTINATION
#     ${CMAKE_CURRENT_BINARY_DIR}/resources/)
