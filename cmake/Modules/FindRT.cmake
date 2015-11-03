if(RT_USE_STATIC_LIBS)
    set(_rt_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

find_library(RT_LIBRARY NAMES rt)

if(RT_USE_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_rt_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

set(RT_LIBRARIES ${RT_LIBRARY})

# Handle the QUIETLY and REQUIRED arguments and set RT_FOUND to true
# if all listed variables are true.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RT DEFAULT_MSG RT_LIBRARY)

mark_as_advanced(RT_LIBRARIES)
