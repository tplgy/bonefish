cmake_minimum_required(VERSION 2.8)
project(bonefish)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror ${CMAKE_CXX_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_GENERATOR}" STREQUAL "Ninja")
    set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

set(Boost_USE_STATIC_LIBS ON)
set(Msgpack_USE_STATIC_LIBS ON)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_subdirectory(src)
add_subdirectory(test)
