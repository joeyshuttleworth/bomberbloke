cmake_minimum_required(VERSION 3.10)
project(BOMBERBLOKE)

if(NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

enable_testing()

if (CMAKE_COMPILER_IS_MINGW)
  add_definitions(-DWC_NO_BEST_FIT_CHARS=0x400)   # Note - removed -DPOCO_WIN32_UTF8
  add_definitions(-D_WIN32 -DMINGW32 -DWIN32_LEAN_AND_MEAN -DWINVER=0x500 -DODBCVER=0x0300 -DPOCO_THREAD_STACK_SIZE)
  add_compile_options(-Wa,-mbig-obj)   # Note: new - fixes "file too big"
endif (CMAKE_COMPILER_IS_MINGW)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  include(CTest)
  add_subdirectory(tests)
endif()
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

if(_WIN32)
set(CMAKE_FIND_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/windows_libraries)
include_directories(${PROJECT_SOURCE_DIR}/windows_libraries/include)
include_directories(/usr/x86_64-w64-mingw32/include)
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -lwsock32 -lws2_32 -lwinmm -lwldap32 -lhid -lsetupapi -lole32 -loleaut32 -limm32 -lversion -mwindows")
endif(_WIN32)

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIR})

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

find_package(curl REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

#Include directories
include_directories(BEFORE SYSTEM ${PROJECT_SOURCE_DIR}/cereal/include)

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
include_directories(${PROJECT_SOURCE_DIR}/engine/text)
include_directories(${PROJECT_SOURCE_DIR}/engine/hud)

# Format in clang
# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -format-style="mozilla"
#   -checks=*;)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sources.cmake SHARED_SOURCES)

# file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests.cmake TEST_SOURCES)

add_library(${LIB_TARGET_NAME} CLIENT ${SHARED_SOURCES} "engine/client.cpp" "engine/network/NetClient.cpp")

# run_and_close test
add_executable(run_and_close "tests/run_and_close.cpp" "engine/client.cpp")
target_link_libraries(run_and_close CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})
add_test(RUN_AND_CLOSE run_and_close COMMAND)

#query_event_test
add_executable(query_event_test "tests/query_event_test.cpp")
target_link_libraries(query_event_test CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES} )
if(_WIN32)
  target_link_libraries(query_event_test ws2_32)
endif(_WIN32)
add_test(QUERY_TEST query_event_test COMMAND)

#local_server_connect test
add_executable(local_server_connect "tests/local_server_connect.cpp")
target_link_libraries(local_server_connect CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})
add_test(LOCAL_SERVER_CONNECT local_server_connect COMMAND)

#test split_to_tokens
add_executable(tokens_test "tests/split_to_tokens_test.cpp")
target_link_libraries(tokens_test CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})
add_test(TEST_SPLIT_TO_TOKENS tokens_test COMMAND)

# explosion demo
add_executable(explosion_demo "tests/ExplosionDemo.cpp" "engine/client.cpp")
target_link_libraries(explosion_demo CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})

add_executable(collision_sandbox "tests/collision_sandbox.cpp" "engine/server.cpp")
target_link_libraries(collision_sandbox SERVER ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})

# Client setup
set(client_src "client/bomberbloke.cpp")
add_executable(bomberbloke_client ${client_src} "engine/client.cpp")
target_link_libraries(bomberbloke_client CLIENT ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})

# Server setup
add_library(${LIB_TARGET_NAME} SERVER ${SHARED_SOURCES} "engine/server.cpp" "engine/network/NetServer.cpp")
set(server_src "server/bomberbloke_server.cpp" "engine/server.cpp" "engine/network/NetServer.cpp")
add_executable(bomberbloke_server ${server_src})
set_target_properties(bomberbloke_server PROPERTIES PUBLIC_HEADER "engine/server.h")
target_link_libraries(bomberbloke_server SERVER ${ENET_LIBRARIES} SDL3::SDL3 ${CURL_LIBRARIES})

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
find_program(CLANG_TIDY "clang-tidy")

# Warning messages are treated as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")

# Build docs
find_package(Doxygen)
set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/.doxygen.conf)
message(WARNING ${PROJECT_SOURCE_DIR})
if(DOXYGEN_FOUND)
  add_custom_target(
    docs ALL
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_IN}
    COMMENT "Generating documentation"
    VERBATIM
    )
endif()

