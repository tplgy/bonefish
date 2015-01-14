find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Threads)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS})

set(SOURCES router.cpp)

add_executable(router ${SOURCES})

add_dependencies(router bonefish)

target_link_libraries(router
    bonefish
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})
