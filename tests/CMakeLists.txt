function(add_connector_test name sources)
    message(STATUS "Registering connector test: " ${name})

    add_executable(${name} ${sources})
    target_link_libraries(${name} CLIENT)

    string(CONCAT TestName ${name} "_TEST")
    add_test(${TestName} ${name})
endfunction()

function(add_client_test name sources)
    message(STATUS "Registering test: " ${name})

    add_executable(${name} ${sources})
    target_link_libraries(${name} CLIENT)

    string(CONCAT TestName ${name} "_TEST")
    add_test(${TestName} ${name})
endfunction()

# Testing the connector implementation in order of increasing complexity
add_connector_test(open_server "connector/open_server.cpp")
add_connector_test(client_connect_server "connector/client_connect_server.cpp")
add_connector_test(multi_client_server "connector/multi_client_server.cpp")
add_connector_test(client_send_message_server "connector/client_send_message_server.cpp")
add_connector_test(client_send_many_message "connector/client_send_many_message.cpp")
add_connector_test(leave_event_server "connector/leave_event_server.cpp")
add_connector_test(client_kicked_event "connector/client_kicked_event.cpp")

# Tests
# Build and discover unit tests
file(GLOB TEST_FILES CONFIGURE_DEPENDS unit/*.cpp)

if(TEST_FILES)
    message(STATUS "Found test files: ${TEST_FILES}")
else()
    message(WARNING "No unit tests found in 'unit/' folder!")
endif()

include_directories(tests)
include(Catch)
enable_testing()
foreach(test_file ${TEST_FILES})
    # Extract the filename without extension
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE CLIENT SERVER ${SDL2_LIBRARIES} ${CURL_LIBRARIES} NET_LIB Catch2::Catch2WithMain)
    target_compile_features(${test_name} PRIVATE cxx_std_17)
    string(CONCAT TestName ${test_name} "_TEST")
    message(STATUS "Registering unit test: " ${TestName})
    add_test(NAME ${TestName} COMMAND ${test_name})
endforeach()

