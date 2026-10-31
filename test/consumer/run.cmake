# CTest -P script backing the `test_consumer` test:
# 1) stage-install the just-built project into a scratch prefix
# 2) configure/build test/consumer with find_package(bonefish CONFIG)
# 3) run the consumer binary
#
# Inputs passed with -D: MAIN_BUILD, STAGE, REPO, BUILD_TYPE, TOOLCHAIN, COMPILER

if(NOT MAIN_BUILD OR NOT STAGE OR NOT REPO)
    message(FATAL_ERROR "test_consumer: MAIN_BUILD/STAGE/REPO must be set")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --install ${MAIN_BUILD} --prefix ${STAGE}
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err)
if(NOT rc STREQUAL 0)
    message(FATAL_ERROR "test_consumer: cmake --install failed (${rc}):\n${out}${err}")
endif()

set(extra "")
if(DEFINED GCC11_ROOT AND NOT GCC11_ROOT STREQUAL "")
    set(ENV{GCC11_ROOT} "${GCC11_ROOT}")
endif()
if(DEFINED TOOLCHAIN AND NOT TOOLCHAIN STREQUAL "")
    list(APPEND extra "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN}")
endif()
if(DEFINED COMPILER AND NOT COMPILER STREQUAL "")
    list(APPEND extra "-DCMAKE_CXX_COMPILER=${COMPILER}")
endif()
# Inherit sanitizer/architecture flags so the consumer links cleanly against
# a sanitizer-instrumented libbonefish.
if(DEFINED CXX_FLAGS AND NOT CXX_FLAGS STREQUAL "")
    list(APPEND extra "-DCMAKE_CXX_FLAGS=${CXX_FLAGS}")
endif()
if(DEFINED EXE_LINKER_FLAGS AND NOT EXE_LINKER_FLAGS STREQUAL "")
    list(APPEND extra "-DCMAKE_EXE_LINKER_FLAGS=${EXE_LINKER_FLAGS}")
endif()

set(consumer_build ${MAIN_BUILD}/consumer-build)

execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${REPO}/test/consumer -B ${consumer_build}
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${STAGE}
        ${extra}
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err)
if(NOT rc STREQUAL 0)
    message(FATAL_ERROR "test_consumer: consumer configure failed (${rc}):\n${out}${err}")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${consumer_build}
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err)
if(NOT rc STREQUAL 0)
    message(FATAL_ERROR "test_consumer: consumer build failed (${rc}):\n${out}${err}")
endif()

find_program(consumer_exe NAMES consumer PATHS ${consumer_build} NO_DEFAULT_PATH)
if(NOT consumer_exe)
    message(FATAL_ERROR "test_consumer: consumer executable not found")
endif()

# Match the project's sanitizer recipe (no leak gating) for the smoke run.
set(ENV{ASAN_OPTIONS} "detect_leaks=0")
set(ENV{UBSAN_OPTIONS} "print_stacktrace=1:halt_on_error=0")

execute_process(
    COMMAND ${consumer_exe}
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err)
message(STATUS "test_consumer: ${out}${err}")
if(NOT rc STREQUAL 0)
    message(FATAL_ERROR "test_consumer: consumer smoke test failed (${rc})")
endif()
