find_package(Boost REQUIRED COMPONENTS random system thread)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS})

link_libraries(
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

set(SOURCES client.cpp)

add_executable(client ${SOURCES})
