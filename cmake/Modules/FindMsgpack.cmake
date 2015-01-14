# - Try to find libmsgpack
# Once done this will define
#  Msgpack_FOUND - System has libmsgpack
#  Msgpack_INCLUDE_DIRS - The libmsgpack include directories
#  Msgpack_DEFINITIONS - Compiler switches (including include directories) needed for using libmsgpack
#  Msgpack_LIBRARIES - The libraries needed to use libmsgpack

find_package(PkgConfig QUIET)

if(Msgpack_USE_STATIC_LIBS)
    set( _libmsgpack_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_MSGPACK QUIET libmsgpack)
endif (PKG_CONFIG_FOUND)

find_path(Msgpack_INCLUDE_DIR msgpack.hpp
          HINTS ${PC_MSGPACK_INCLUDEDIR} ${PC_MSGPACK_INCLUDE_DIRS})

find_library(Msgpack_LIBRARY NAMES msgpack
             HINTS ${PC_MSGPACK_LIBDIR} ${PC_MSGPACK_LIBRARY_DIRS})

if(Msgpack_USE_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_libmsgpack_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

set(Msgpack_LIBRARIES ${Msgpack_LIBRARY})
set(Msgpack_INCLUDE_DIRS ${Msgpack_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Msgpack_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Msgpack DEFAULT_MSG
                                  Msgpack_LIBRARY
                                  Msgpack_LIBRARIES
                                  Msgpack_INCLUDE_DIR
                                  Msgpack_INCLUDE_DIRS)

mark_as_advanced(Msgpack_INCLUDE_DIR Msgpack_LIBRARY)
