find_package(Python3 COMPONENTS Interpreter QUIET)

if(Python3_Interpreter_FOUND)
    add_test(NAME e2e_all
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/e2e_driver.py
            --daemon $<TARGET_FILE:bonefish_bin>
            --integration $<TARGET_FILE:integration>)

    set_tests_properties(e2e_all PROPERTIES
        TIMEOUT 300
        RUN_SERIAL TRUE)
else()
    message(STATUS "Python3 interpreter not found - skipping e2e tests")
endif()
