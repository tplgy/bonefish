# - Try to find bonefish
#
# Modern installations export a CMake package (bonefish::bonefish); this
# module simply forwards to it when present. Otherwise it falls back to
# locating a bare libbonefish + headers (note: the vendored third-party
# headers (standalone Asio, websocketpp, ...) must then be provided by
# the caller; define ASIO_STANDALONE).
#
# Once done this will define
#  Bonefish_FOUND - System has bonefish
#  Bonefish_INCLUDE_DIRS - The bonefish include directories
#  Bonefish_LIBRARIES - The libraries needed to use bonefish

find_package(bonefish CONFIG QUIET)
if(bonefish_FOUND)
    set(Bonefish_FOUND TRUE)
    set(Bonefish_LIBRARIES bonefish::bonefish)
    get_target_property(Bonefish_INCLUDE_DIRS bonefish::bonefish
        INTERFACE_INCLUDE_DIRECTORIES)
    mark_as_advanced(Bonefish_FOUND)
    return()
endif()

if(Bonefish_USE_STATIC_LIBS)
    set(_bonefish_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
else()
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_BONEFISH QUIET bonefish)
    endif()
endif()

if(PC_BONEFISH_FOUND)
    set(Bonefish_DEFINITIONS ${PC_BONEFISH_CFLAGS})
endif()

find_path(Bonefish_INCLUDE_DIR bonefish
          HINTS ${PC_BONEFISH_INCLUDEDIR} ${PC_BONEFISH_INCLUDE_DIRS}
          PATH_SUFFIXES bonefish)

find_library(Bonefish_LIBRARY NAMES bonefish
             HINTS ${PC_BONEFISH_LIBDIR} ${PC_BONEFISH_LIBRARY_DIRS})

if(Bonefish_USE_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_bonefish_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

set(Bonefish_LIBRARIES ${Bonefish_LIBRARY})
set(Bonefish_INCLUDE_DIRS ${Bonefish_INCLUDE_DIR})
set(Bonefish_DEFINITIONS ${Bonefish_DEFINITIONS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Bonefish DEFAULT_MSG
                                  Bonefish_LIBRARY
                                  Bonefish_LIBRARIES
                                  Bonefish_INCLUDE_DIR
                                  Bonefish_INCLUDE_DIRS)

mark_as_advanced(Bonefish_INCLUDE_DIR Bonefish_LIBRARY)
