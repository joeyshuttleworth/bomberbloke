cmake_minimum_required(VERSION 3.10)
project(BOMBERBLOKE)

enable_testing()
include(FetchContent)

# Include directories

include_directories(BEFORE SYSTEM ${PROJECT_SOURCE_DIR}/cereal/include)
include_directories(${PROJECT_SOURCE_DIR}/engine/event)
include_directories(${PROJECT_SOURCE_DIR}/engine/file)
include_directories(${PROJECT_SOURCE_DIR}/engine/scene)
include_directories(${PROJECT_SOURCE_DIR}/engine/network)
include_directories(${PROJECT_SOURCE_DIR}/engine/network/connector)
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

# Source files

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sources.cmake SHARED_SOURCES)

if(EMSCRIPTEN)
  message(STATUS "Detected Emscripten, building using CMakeEmscripten.cmake")
  include("CMakeEmscripten.cmake")
  return()
endif()

if (CMAKE_COMPILER_IS_MINGW)
  add_definitions(-DWC_NO_BEST_FIT_CHARS=0x400)   # Note - removed -DPOCO_WIN32_UTF8
  add_definitions(-D_WIN32 -DMINGW32 -DWIN32_LEAN_AND_MEAN -DWINVER=0x500 -DODBCVER=0x0300 -DPOCO_THREAD_STACK_SIZE)
  add_compile_options(-Wa,-mbig-obj)   # Note: new - fixes "file too big"
endif (CMAKE_COMPILER_IS_MINGW)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  include(CTest)
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

# Dependencies

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(curl REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIR})

set(LWS_WITH_IPV6 ON CACHE BOOL "" FORCE)
set(LWS_WITH_HTTP2 ON CACHE BOOL "" FORCE)
set(LWS_WITH_PLUGINS ON CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "" FORCE)
set(LWS_UNIX_SOCK ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  lws
  GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
  GIT_TAG v4.3-stable
)
FetchContent_MakeAvailable(lws)

# Resources

include(CMakeRC)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/assets.cmake ASSET_FILES)
cmrc_add_resource_library(files ALIAS files::rc PREFIX files ${ASSET_FILES})

# Networking setup, the client and server can be linked with NET_LIB

set(NET_PROTOCOL ENET CACHE STRING "")
set(NET_PROTOCOL_OPTIONS ENET WEBSOCKETS)
if (NOT NET_PROTOCOL IN_LIST NET_PROTOCOL_OPTIONS)
  message(FATAL_ERROR "Unrecognised network protocol for build.")
endif()

message(STATUS "Using connector " ${NET_PROTOCOL})

add_library(NET_LIB INTERFACE)
if(NET_PROTOCOL STREQUAL "ENET")
  target_link_libraries(NET_LIB INTERFACE ${ENET_LIBRARIES})
  list(APPEND SHARED_SOURCES "engine/network/connector/ENetConnector.cpp")
else()
  target_link_libraries(NET_LIB INTERFACE websockets)
  list(APPEND SHARED_SOURCES "engine/network/connector/WebSocketsConnector.cpp")
endif()

#
# Build targets declared here
#

add_library(CLIENT ${SHARED_SOURCES} "engine/client.cpp" "engine/network/NetClient.cpp")
target_link_libraries(CLIENT PUBLIC files::rc ${SDL2_LIBRARIES} ${CURL_LIBRARIES} NET_LIB)

# Tests
add_subdirectory(tests)

# Client 
set(client_src "client/bomberbloke.cpp")
add_executable(bomberbloke_client ${client_src} "engine/client.cpp")
target_link_libraries(bomberbloke_client PUBLIC files::rc CLIENT)
# Server
add_library(SERVER ${SHARED_SOURCES} "engine/server.cpp" "engine/network/NetServer.cpp")
target_link_libraries(SERVER PUBLIC files::rc NET_LIB)
set(server_src "server/bomberbloke_server.cpp" "engine/server.cpp" "engine/network/NetServer.cpp")
add_executable(bomberbloke_server ${server_src})
set_target_properties(bomberbloke_server PROPERTIES PUBLIC_HEADER "engine/server.h")
target_link_libraries(bomberbloke_server SERVER ${SDL2_LIBRARIES} ${CURL_LIBRARIES} NET_LIB)

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
find_program(CLANG_TIDY "clang-tidy")

# Warning messages are treated as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")

# # Build docs
# find_package(Doxygen)
# set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/.doxygen.conf)
# if(DOXYGEN_FOUND)
#   add_custom_target(
#     docs ALL
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_IN}
#     COMMENT "Generating documentation"
#     VERBATIM
#     )
# endif()

