function(add_connector_test name sources)
    message(STATUS "Registering connector test: " ${name})

    add_executable(${name} ${sources})
    target_link_libraries(${name} CLIENT ${CURL_LIBRARIES})

    string(CONCAT TestName ${name} "_TEST")
    add_test(${TestName} ${name})
endfunction()

function(add_client_test name sources)
    message(STATUS "Registering test: " ${name})

    add_executable(${name} ${sources})
    target_link_libraries(${name} CLIENT ${CURL_LIBRARIES})

    string(CONCAT TestName ${name} "_TEST")
    add_test(${TestName} ${name})
endfunction()

# Client tests
add_client_test(run_and_close "run_and_close.cpp")
add_client_test(serialise_event "serialise_event_test.cpp")
add_client_test(tokens_test "split_to_tokens_test.cpp")
add_client_test(explosion_demo "ExplosionDemo.cpp")
add_client_test(collision_sandbox "collision_sandbox.cpp")

# Testing the connector implementation in order of increasing complexity
# TODO split these connectors by process as LWS acts funny with two threads, two *context
add_connector_test(open_server "connector/open_server.cpp")
add_connector_test(client_connect_server "connector/client_connect_server.cpp")
add_connector_test(multi_client_server "connector/multi_client_server.cpp")
add_connector_test(client_send_message_server "connector/client_send_message_server.cpp")
add_connector_test(client_send_many_message "connector/client_send_many_message.cpp")
add_connector_test(leave_event_server "connector/leave_event_server.cpp")
add_connector_test(client_kicked_event "connector/client_kicked_event.cpp")