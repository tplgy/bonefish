# Toolchain file for portability builds with an Ubuntu 22.04 (jammy) GCC 11
# toolchain extracted from .deb packages, e.g.:
#
#   GCC11_ROOT=/path/to/gcc11 cmake -S . -B build-gcc11 \
#       -DCMAKE_TOOLCHAIN_FILE=test/portability/gcc11-toolchain.cmake
#
# GCC11_ROOT must contain usr/bin/g++-11 (e.g. from g++-11_11.4.0-1ubuntu1~22.04*).
if(DEFINED ENV{GCC11_ROOT})
    set(_gcc11_root "$ENV{GCC11_ROOT}")
else()
    message(FATAL_ERROR "GCC11_ROOT environment variable is not set")
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER "${_gcc11_root}/usr/bin/gcc-11")
set(CMAKE_CXX_COMPILER "${_gcc11_root}/usr/bin/g++-11")
set(CMAKE_FIND_ROOT_PATH "${_gcc11_root}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
