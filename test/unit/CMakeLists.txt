set(UNIT_TESTS
    test_messages
    test_wamp_uri
    test_trace
    test_native_disconnect)

foreach(test_name ${UNIT_TESTS})
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} bonefish)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
