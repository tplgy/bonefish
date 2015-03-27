cmake_minimum_required(VERSION 2.8)
project(bonefish)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror ${CMAKE_CXX_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
    if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

option(shared "build bonefish as a shared library" OFF)
option(asio_source "include Boost.Asio implementation (src.hpp) in library" ON)

if(shared)
    set(Boost_USE_STATIC_LIBS OFF)
    add_definitions(-DBOOST_ALL_DYN_LINK)
else()
    set(Boost_USE_STATIC_LIBS ON)
    add_definitions(-DBOOST_ASIO_SEPARATE_COMPILATION)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/third-party/msgpack-c/include
    ${CMAKE_SOURCE_DIR}/third-party/rapidjson/include
    ${CMAKE_SOURCE_DIR}/third-party/websocketpp
)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(bin)
add_subdirectory(test)
