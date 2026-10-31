cmake_minimum_required(VERSION 3.16)
project(bonefish VERSION 1.0.0 LANGUAGES CXX)

option(shared "build bonefish as a shared library" OFF)
option(stdlib "When building with clang, you can choose libc++ (default) or libstdc++" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (NOT stdlib)
        set(stdlib "libc++")
    endif()
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
    if (stdlib STREQUAL "libstdc++")
        # libstdc++ defines template<typename> friend class hash, which causes warnings
        set(CMAKE_CXX_FLAGS "-Wno-mismatched-tags ${CMAKE_CXX_FLAGS}")
    endif()
    set(CMAKE_CXX_FLAGS "-stdlib=${stdlib} -Wno-unused-local-typedefs -Wno-unknown-warning-option ${CMAKE_CXX_FLAGS}")
else()
    if(stdlib)
        message(STATUS "stdlib option (set to ${stdlib}) is ignored for ${CMAKE_CXX_COMPILER_ID} compiler")
    endif()
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "-fdiagnostics-color=auto ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if(NOT MSVC)
    add_compile_options(-Wall -Werror -Wno-unused-variable)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "-O0 ${CMAKE_C_FLAGS_DEBUG}")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0500)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

find_package(Threads REQUIRED)

# Standalone Asio (vendored, header-only). websocketpp is configured for it
# via ASIO_STANDALONE.
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/asio/asio/include")
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
if(WIN32)
    target_link_libraries(asio INTERFACE ws2_32 mswsock)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

# Third-party headers are included as SYSTEM headers so that their warnings
# are not escalated by -Wall -Werror.
include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/third-party/cppcodec
    ${CMAKE_SOURCE_DIR}/third-party/json-msgpack/include
    ${CMAKE_SOURCE_DIR}/third-party/msgpack-c/include
    ${CMAKE_SOURCE_DIR}/third-party/rapidjson/include
    ${CMAKE_SOURCE_DIR}/third-party/websocketpp
)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

enable_testing()

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(daemon)
add_subdirectory(test)
