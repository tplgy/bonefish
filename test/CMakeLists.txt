add_subdirectory(functional)
add_subdirectory(unit)
add_subdirectory(e2e)

# Independent-project consumption test: stage-installs the build tree,
# then configures/builds/runs test/consumer against the installed package.
add_test(NAME test_consumer
    COMMAND ${CMAKE_COMMAND}
        -DMAIN_BUILD=${CMAKE_BINARY_DIR}
        -DSTAGE=${CMAKE_BINARY_DIR}/stage-install
        -DREPO=${CMAKE_SOURCE_DIR}
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DTOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}
        -DCOMPILER=${CMAKE_CXX_COMPILER}
        "-DGCC11_ROOT=$ENV{GCC11_ROOT}"
        "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
        "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/consumer/run.cmake)
set_tests_properties(test_consumer PROPERTIES
    TIMEOUT 600
    RUN_SERIAL TRUE)
