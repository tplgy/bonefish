# Standalone consumer project: proves the installed CMake package
# (bonefish::bonefish) works from an independent project. Built by the
# `test_consumer` CTest via consumer/run.cmake, but can also be driven
# manually:
#
#   cmake -S test/consumer -B build-consumer -DCMAKE_PREFIX_PATH=<install-prefix>

cmake_minimum_required(VERSION 3.16)
project(bonefish_consumer_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(bonefish REQUIRED)

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE bonefish::bonefish)
