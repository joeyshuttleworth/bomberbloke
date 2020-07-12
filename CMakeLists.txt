cmake_minimum_required(VERSION 3.10)
project(BOMBERBLOKE)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  include(CTest)
  add_subdirectory(tests)
endif()
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(curl REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

#Include directories
include_directories(${PROJECT_SOURCE_DIR}/engine/event)
include_directories(${PROJECT_SOURCE_DIR}/engine/file)
include_directories(${PROJECT_SOURCE_DIR}/engine/scene)
include_directories(${PROJECT_SOURCE_DIR}/engine/network)
include_directories(${PROJECT_SOURCE_DIR}/engine/player)
include_directories(${PROJECT_SOURCE_DIR}/engine/actor)
include_directories(${PROJECT_SOURCE_DIR}/engine/control)
include_directories(${PROJECT_SOURCE_DIR}/engine/collider)
include_directories(${PROJECT_SOURCE_DIR}/client)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/engine)
include_directories(${PROJECT_SOURCE_DIR}/tests)
include_directories(${PROJECT_SOURCE_DIR}/engine/animation)
include_directories(${PROJECT_SOURCE_DIR}/engine/camera)
include_directories(${PROJECT_SOURCE_DIR}/engine/sound)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sources.cmake SHARED_SOURCES)

# file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests.cmake TEST_SOURCES)

add_library(${LIB_TARGET_NAME} CLIENT ${SHARED_SOURCES} "engine/client.cpp" "engine/network/NetClient.cpp")

# run_and_close test
add_executable(run_and_close "tests/run_and_close.cpp" "engine/client.cpp")
target_link_libraries(run_and_close CLIENT ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})
add_test(RUN_AND_CLOSE run_and_close COMMAND)

#query_event_test
add_executable(query_event_test "tests/query_event_test.cpp")
target_link_libraries(query_event_test CLIENT ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})
add_test(QUERY_TEST query_event_test COMMAND)

#local_server_connect test
add_executable(local_server_connect "tests/local_server_connect.cpp")
target_link_libraries(local_server_connect CLIENT ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})
add_test(LOCAL_SERVER_CONNECT local_server_connect COMMAND)

# explosion demo
add_executable(explosion_demo "tests/ExplosionDemo.cpp" "engine/client.cpp")
target_link_libraries(explosion_demo CLIENT ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})

add_executable(collision_sandbox "tests/collision_sandbox.cpp" "engine/server.cpp")
target_link_libraries(collision_sandbox SERVER ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})

# Client setup
set(client_src "client/bomberbloke.cpp")
add_executable(bomberbloke_client ${client_src} "engine/client.cpp")
target_link_libraries(bomberbloke_client CLIENT ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})

# Server setup
add_library(${LIB_TARGET_NAME} SERVER ${SHARED_SOURCES} "engine/server.cpp" "engine/network/NetServer.cpp")
set(server_src "server/bomberbloke_server.cpp" "engine/server.cpp" "engine/network/NetServer.cpp")
add_executable(bomberbloke_server ${server_src})
set_target_properties(bomberbloke_server PROPERTIES PUBLIC_HEADER "engine/server.h")
target_link_libraries(bomberbloke_server SERVER ${ENET_LIBRARIES} ${SDL2_LIBRARIES} ${CURL_LIBRARIES})


# Format in clang
file(GLOB_RECURSE all_sources include/*.h src/*.cpp)
add_custom_target(fmt
    COMMAND clang-format -style=Mozzila -i ${all_sources})



# Warning messages are treated as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")
